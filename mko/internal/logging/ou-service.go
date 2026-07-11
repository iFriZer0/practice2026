package logging

import (
	"context"
	"time"

	"mko/internal/domain"
	"mko/internal/service"

	"go.uber.org/zap"
)

// ouService — логирующий декоратор над service.OuService, построенный
// по тому же принципу, что и kkService в этом пакете: сама
// бизнес-логика (internal/service/ou-usecase.go) ничего не знает про
// логирование, а весь ОУ-трафик проходит здесь через единую точку
// (это закрывает пункт ТЗ "логирование работы").
type ouService struct {
	next service.OuService
	log  *zap.Logger
}

// NewOuService оборачивает next логирующим декоратором на базе zap.
// Использование: logging.NewOuService(service.NewOuService(driver), logger).
func NewOuService(next service.OuService, log *zap.Logger) service.OuService {
	return &ouService{
		next: next,
		log:  loggerOrNop(log).Named("ou_service"),
	}
}

func (s *ouService) GetVersion(ctx context.Context) (string, error) {
	start := time.Now()
	fields := operationFields("GetVersion", start)

	s.log.Debug("GetVersion request received", fields...)
	version, err := s.next.GetVersion(ctx)
	s.logResult("GetVersion", start, append(fields, zap.String("version", version)), err)
	return version, err
}

func (s *ouService) Ping(ctx context.Context) error {
	start := time.Now()
	fields := operationFields("Ping", start)

	s.log.Debug("Ping request received", fields...)
	err := s.next.Ping(ctx)
	s.logResult("Ping", start, fields, err)
	return err
}

func (s *ouService) ConfigureOu(ctx context.Context, cfg domain.OuConfig) error {
	start := time.Now()
	fields := append(baseOuConfigFields(cfg), operationFields("ConfigureOu", start)...)

	s.log.Info("ConfigureOu request received", fields...)
	err := s.next.ConfigureOu(ctx, cfg)
	s.logResult("ConfigureOu", start, fields, err)
	return err
}

func (s *ouService) SetOuResponseWord(ctx context.Context, board domain.BoardID, index domain.McoIndex, channel domain.Channel, ouAddress int32, word int32) error {
	start := time.Now()
	fields := append(
		[]zap.Field{
			zap.String("board_id", string(board)),
			zap.Int32("mko_index", int32(index)),
			zap.Int32("channel", int32(channel)),
			zap.Int32("ou_address", ouAddress),
			zap.Int32("response_word", word),
		},
		operationFields("SetOuResponseWord", start)...,
	)

	s.log.Info("SetOuResponseWord request received", fields...)
	err := s.next.SetOuResponseWord(ctx, board, index, channel, ouAddress, word)
	s.logResult("SetOuResponseWord", start, fields, err)
	return err
}

func (s *ouService) ReadOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, receiveArea bool) (domain.OuSubaddressData, error) {
	start := time.Now()
	fields := append(
		[]zap.Field{
			zap.String("board_id", string(board)),
			zap.Uint32("subaddress", sub),
			zap.Bool("receive_area", receiveArea),
		},
		operationFields("ReadOuSubaddress", start)...,
	)

	s.log.Info("ReadOuSubaddress request received", fields...)
	data, err := s.next.ReadOuSubaddress(ctx, board, sub, receiveArea)
	if err != nil {
		s.logResult("ReadOuSubaddress", start, fields, err)
		return data, err
	}
	s.logResult("ReadOuSubaddress", start, append(fields, ouSubaddressDataFields(data)...), nil)
	return data, nil
}

func (s *ouService) WriteOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, sd []uint32) error {
	start := time.Now()
	fields := append(
		[]zap.Field{
			zap.String("board_id", string(board)),
			zap.Uint32("subaddress", sub),
			zap.Int("sd_count", len(sd)),
			zap.Uint32s("sd_words", sd),
		},
		operationFields("WriteOuSubaddress", start)...,
	)

	s.log.Info("WriteOuSubaddress request received", fields...)
	err := s.next.WriteOuSubaddress(ctx, board, sub, sd)
	s.logResult("WriteOuSubaddress", start, fields, err)
	return err
}

// SubscribeOuCommands логирует открытие/закрытие потока и каждое
// входящее событие ОУ — по тому же шаблону, что и
// kkService.SubscribeExchangeResults.
func (s *ouService) SubscribeOuCommands(ctx context.Context, board domain.BoardID) (<-chan domain.OuCommandEvent, error) {
	start := time.Now()
	fields := append(
		[]zap.Field{zap.String("board_id", string(board))},
		operationFields("SubscribeOuCommands", start)...,
	)

	s.log.Info("SubscribeOuCommands request received", fields...)
	events, err := s.next.SubscribeOuCommands(ctx, board)
	if err != nil {
		s.logResult("SubscribeOuCommands", start, fields, err)
		return nil, err
	}

	out := make(chan domain.OuCommandEvent)
	go func() {
		defer close(out)
		defer s.log.Info(
			"SubscribeOuCommands stream closed",
			append(fields, zap.Duration("stream_duration", time.Since(start)))...,
		)

		for {
			select {
			case ev, ok := <-events:
				if !ok {
					return
				}

				s.log.Info("ou command event received from driver", ouCommandEventFields(ev)...)

				select {
				case out <- ev:
					s.log.Debug("ou command event forwarded to transport", ouCommandEventFields(ev)...)
				case <-ctx.Done():
					s.log.Warn("SubscribeOuCommands context done while forwarding", append(fields, zap.Error(ctx.Err()))...)
					return
				}
			case <-ctx.Done():
				s.log.Warn("SubscribeOuCommands context done", append(fields, zap.Error(ctx.Err()))...)
				return
			}
		}
	}()

	s.logResult("SubscribeOuCommands", start, fields, nil)
	return out, nil
}

func (s *ouService) logResult(method string, start time.Time, fields []zap.Field, err error) {
	resultFields := append(fields, zap.Duration("duration", time.Since(start)))
	if err != nil {
		s.log.Error(method+" failed", append(resultFields, zap.Error(err))...)
		return
	}
	s.log.Info(method+" completed", resultFields...)
}

func baseOuConfigFields(cfg domain.OuConfig) []zap.Field {
	return []zap.Field{
		zap.String("board_id", string(cfg.Board)),
		zap.Int32("mko_index", int32(cfg.Index)),
		zap.Int32("channel", int32(cfg.Channel)),
		zap.Int32("ou_address", cfg.OuAddress),
		zap.Int32("response_word", cfg.ResponseWord),
		zap.String("remote_ip", cfg.RemoteIP),
		zap.Int32("remote_port", cfg.RemotePort),
	}
}

func ouSubaddressDataFields(data domain.OuSubaddressData) []zap.Field {
	return []zap.Field{
		zap.Uint32("cmd_word", data.CmdWord),
		zap.Uint32("result_word", data.ResultWord),
		zap.Int("sd_count", len(data.Sd)),
		zap.Uint32s("sd_words", data.Sd),
		zap.String("decoded_result", data.DecodedResult),
	}
}

func ouCommandEventFields(ev domain.OuCommandEvent) []zap.Field {
	return []zap.Field{
		zap.String("board_id", string(ev.Board)),
		zap.Uint32("cmd_word", ev.CmdWord),
		zap.Uint32("result_word", ev.ResultWord),
		zap.Bool("receive_from_ou", ev.ReceiveFromOu),
		zap.Uint32("ou_address", ev.OuAddress),
		zap.Uint32("subaddress", ev.Subaddress),
		zap.Uint32("word_count", ev.WordCount),
		zap.String("decoded_command", ev.DecodedCommand),
		zap.String("decoded_result", ev.DecodedResult),
	}
}
