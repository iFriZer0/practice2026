package logging

import (
	"context"
	"os"
	"strings"
	"time"

	"mko/internal/domain"
	"mko/internal/service"

	"go.uber.org/zap"
	"go.uber.org/zap/zapcore"
)

const (
	defaultLogLevel  = "info"
	defaultLogFormat = "console"
	defaultLogEnv    = "local"
)

type kkService struct {
	next service.KKService
	log  *zap.Logger
}

func NewZapLogger() (*zap.Logger, error) {
	level := zap.NewAtomicLevelAt(parseLevel(envOrDefault("LOG_LEVEL", defaultLogLevel)))

	cfg := zap.NewProductionConfig()
	cfg.Level = level
	cfg.Encoding = envOrDefault("LOG_FORMAT", defaultLogFormat)
	cfg.OutputPaths = []string{"stdout"}
	cfg.ErrorOutputPaths = []string{"stderr"}
	cfg.EncoderConfig.TimeKey = "ts"
	cfg.EncoderConfig.LevelKey = "level"
	cfg.EncoderConfig.MessageKey = "msg"
	cfg.EncoderConfig.CallerKey = "caller"
	cfg.EncoderConfig.EncodeTime = zapcore.ISO8601TimeEncoder
	cfg.EncoderConfig.EncodeLevel = zapcore.LowercaseLevelEncoder

	if envOrDefault("LOG_ENV", defaultLogEnv) == "local" {
		cfg.Development = true
		cfg.EncoderConfig.EncodeLevel = zapcore.CapitalColorLevelEncoder
	}

	return cfg.Build(zap.AddCaller(), zap.AddCallerSkip(1))
}

func NewKKService(next service.KKService, log *zap.Logger) service.KKService {
	return &kkService{
		next: next,
		log:  loggerOrNop(log).Named("kk_service"),
	}
}

func (s *kkService) ConfigureKK(ctx context.Context, cfg domain.KKConfig) error {
	start := time.Now()
	fields := append(baseKKConfigFields(cfg), operationFields("ConfigureKK", start)...)

	s.log.Info("ConfigureKK request received", fields...)
	err := s.next.ConfigureKK(ctx, cfg)
	s.logResult("ConfigureKK", start, fields, err)
	return err
}

func (s *kkService) ConfigureExchange(ctx context.Context, cfg domain.ExchangeConfig) error {
	start := time.Now()
	fields := append(baseExchangeConfigFields(cfg), operationFields("ConfigureExchange", start)...)

	s.log.Info("ConfigureExchange request received", fields...)
	err := s.next.ConfigureExchange(ctx, cfg)
	s.logResult("ConfigureExchange", start, fields, err)
	return err
}

func (s *kkService) RunExchange(ctx context.Context, cmd domain.RunExchangeCommand) error {
	start := time.Now()
	fields := append(baseRunExchangeFields(cmd), operationFields("RunExchange", start)...)

	s.log.Info("RunExchange request received", fields...)
	err := s.next.RunExchange(ctx, cmd)
	s.logResult("RunExchange", start, fields, err)
	return err
}

func (s *kkService) SubscribeExchangeResults(ctx context.Context, boardID string) (<-chan domain.ExchangeResult, error) {
	start := time.Now()
	fields := append(
		[]zap.Field{zap.String("board_id", boardID)},
		operationFields("SubscribeExchangeResults", start)...,
	)

	s.log.Info("SubscribeExchangeResults request received", fields...)
	results, err := s.next.SubscribeExchangeResults(ctx, boardID)
	if err != nil {
		s.logResult("SubscribeExchangeResults", start, fields, err)
		return nil, err
	}

	out := make(chan domain.ExchangeResult)
	go func() {
		defer close(out)
		defer s.log.Info(
			"SubscribeExchangeResults stream closed",
			append(fields, zap.Duration("stream_duration", time.Since(start)))...,
		)

		for {
			select {
			case result, ok := <-results:
				if !ok {
					return
				}

				s.log.Info("exchange result received from driver", exchangeResultFields(result)...)

				select {
				case out <- result:
					s.log.Debug("exchange result forwarded to transport", exchangeResultFields(result)...)
				case <-ctx.Done():
					s.log.Warn("SubscribeExchangeResults context done while forwarding", append(fields, zap.Error(ctx.Err()))...)
					return
				}
			case <-ctx.Done():
				s.log.Warn("SubscribeExchangeResults context done", append(fields, zap.Error(ctx.Err()))...)
				return
			}
		}
	}()

	s.logResult("SubscribeExchangeResults", start, fields, nil)
	return out, nil
}

func (s *kkService) logResult(method string, start time.Time, fields []zap.Field, err error) {
	resultFields := append(fields, zap.Duration("duration", time.Since(start)))
	if err != nil {
		s.log.Error(method+" failed", append(resultFields, zap.Error(err))...)
		return
	}
	s.log.Info(method+" completed", resultFields...)
}

func operationFields(method string, start time.Time) []zap.Field {
	return []zap.Field{
		zap.String("method", method),
		zap.Time("started_at", start),
	}
}

func baseKKConfigFields(cfg domain.KKConfig) []zap.Field {
	return []zap.Field{
		zap.String("board_id", string(cfg.Board)),
		zap.String("operation_id", cfg.OperationID),
		zap.Int32("mko_index", int32(cfg.Index)),
		zap.Int32("channel", int32(cfg.Channel)),
		zap.Int32("bus_control", int32(cfg.BusControl)),
		zap.Int32("ou_address", cfg.OuAddress),
		zap.Int32("ou_resp_word", cfg.OuRespWord),
		zap.Int32("vector_word", cfg.VectorWord),
		zap.Int32("selftest_word", cfg.SelftestWord),
		zap.String("remote_ip", cfg.RemoteIP),
		zap.Int32("remote_port", cfg.RemotePort),
	}
}

func baseExchangeConfigFields(cfg domain.ExchangeConfig) []zap.Field {
	return []zap.Field{
		zap.String("board_id", string(cfg.Board)),
		zap.String("operation_id", cfg.OperationID),
		zap.Int32("format", cfg.Format),
		zap.Uint32("ks1", cfg.KS1),
		zap.Uint32("ks2", cfg.KS2),
		zap.Int("sd_count", len(cfg.SD)),
		zap.Uint32s("sd_words", cfg.SD),
	}
}

func baseRunExchangeFields(cmd domain.RunExchangeCommand) []zap.Field {
	return []zap.Field{
		zap.String("board_id", string(cmd.Board)),
		zap.String("operation_id", cmd.OperationID),
	}
}

func exchangeResultFields(result domain.ExchangeResult) []zap.Field {
	return []zap.Field{
		zap.String("board_id", string(result.Board)),
		zap.Int32("format", result.Format),
		zap.Uint32("ks1", result.KS1),
		zap.Uint32("ks2", result.KS2),
		zap.Int("sd_count", len(result.SD)),
		zap.Uint32s("sd_words", result.SD),
		zap.Uint32("answer_word_1", result.AnswerWord1),
		zap.Uint32("answer_word_2", result.AnswerWord2),
		zap.Uint32("result_word", result.ResultWord),
		zap.String("decoded_result", result.DecodedResult),
	}
}

func loggerOrNop(log *zap.Logger) *zap.Logger {
	if log == nil {
		return zap.NewNop()
	}
	return log
}

func parseLevel(raw string) zapcore.Level {
	switch strings.ToLower(strings.TrimSpace(raw)) {
	case "debug":
		return zapcore.DebugLevel
	case "warn", "warning":
		return zapcore.WarnLevel
	case "error":
		return zapcore.ErrorLevel
	default:
		return zapcore.InfoLevel
	}
}

func envOrDefault(key, fallback string) string {
	value := os.Getenv(key)
	if value == "" {
		return fallback
	}
	return value
}
