package service

import (
	"context"
	"log/slog"
	"mko/internal/domain"
)

type DriverOuClient interface {
	GetVersion(ctx context.Context) (string, error)
	Ping(ctx context.Context) error

	ConfigureOu(ctx context.Context, cfg domain.OuConfig) error
	SetOuResponseWord(ctx context.Context, board domain.BoardID, index domain.McoIndex, channel domain.Channel, ouAddress int32, word int32) error
	ReadOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, receiveArea bool) (domain.OuSubaddressData, error)
	WriteOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, sd []uint32) error
	SubscribeOuCommands(ctx context.Context, board domain.BoardID) (<-chan domain.OuCommandEvent, error)
}

// OuService реализует transport.OuServiceInterface. Каждая операция
// логируется здесь централизованно — это единственная точка, через
// которую проходят все вызовы к ОУ, что закрывает пункт ТЗ
// "логирование работы" в одном месте, а не размазывает его по
// transport и по transport/clients.
type OuService struct {
	driver DriverOuClient
	log    *slog.Logger
}

func NewOuService(driver DriverOuClient, log *slog.Logger) *OuService {
	if log == nil {
		log = slog.Default()
	}
	return &OuService{driver: driver, log: log}
}

func (u *OuService) GetVersion(ctx context.Context) (string, error) {
	v, err := u.driver.GetVersion(ctx)
	u.logCall(ctx, "GetVersion", nil, err)
	return v, err
}

func (u *OuService) Ping(ctx context.Context) error {
	err := u.driver.Ping(ctx)
	u.logCall(ctx, "Ping", nil, err)
	return err
}

func (u *OuService) ConfigureOu(ctx context.Context, cfg domain.OuConfig) error {
	// Нормализация по правилам протокола МКО: драйвер сам приводит
	// index<=0 к 1 (см. документацию mko_driver, п.5.2), но делает
	// это молча внутри себя. Явно повторяем это правило здесь, а не
	// полагаемся на скрытое поведение драйвера — так поведение видно
	// в логах и тестируемо на уровне бизнес-логики.
	if cfg.Index <= 0 {
		u.log.WarnContext(ctx, "ou index <= 0, normalizing to 1", "board", cfg.Board, "requested_index", cfg.Index)
		cfg.Index = 1
	}

	err := u.driver.ConfigureOu(ctx, cfg)
	u.logCall(ctx, "ConfigureOu", map[string]any{"board": cfg.Board, "ou_address": cfg.OuAddress}, err)
	return err
}

func (u *OuService) SetOuResponseWord(ctx context.Context, board domain.BoardID, index domain.McoIndex, channel domain.Channel, ouAddress int32, word int32) error {
	if index <= 0 {
		index = 1
	}
	err := u.driver.SetOuResponseWord(ctx, board, index, channel, ouAddress, word)
	u.logCall(ctx, "SetOuResponseWord", map[string]any{"board": board, "ou_address": ouAddress}, err)
	return err
}

func (u *OuService) ReadOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, receiveArea bool) (domain.OuSubaddressData, error) {
	data, err := u.driver.ReadOuSubaddress(ctx, board, sub, receiveArea)
	u.logCall(ctx, "ReadOuSubaddress", map[string]any{"board": board, "subaddress": sub, "receive_area": receiveArea}, err)
	return data, err
}

func (u *OuService) WriteOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, sd []uint32) error {
	err := u.driver.WriteOuSubaddress(ctx, board, sub, sd)
	u.logCall(ctx, "WriteOuSubaddress", map[string]any{"board": board, "subaddress": sub, "word_count": len(sd)}, err)
	return err
}

// SubscribeOuCommands прокидывает канал от driver-клиента как есть,
// но логирует каждое событие по мере поступления — отдельной
// горутиной, чтобы не задерживать доставку события в transport.
func (u *OuService) SubscribeOuCommands(ctx context.Context, board domain.BoardID) (<-chan domain.OuCommandEvent, error) {
	events, err := u.driver.SubscribeOuCommands(ctx, board)
	u.logCall(ctx, "SubscribeOuCommands", map[string]any{"board": board}, err)
	if err != nil {
		return nil, err
	}

	out := make(chan domain.OuCommandEvent)
	go func() {
		defer close(out)
		for {
			select {
			case ev, ok := <-events:
				if !ok {
					return
				}
				u.log.InfoContext(ctx, "ou command event",
					"board", board,
					"ou_address", ev.OuAddress,
					"subaddress", ev.Subaddress,
					"decoded_command", ev.DecodedCommand,
					"decoded_result", ev.DecodedResult,
				)
				select {
				case out <- ev:
				case <-ctx.Done():
					return
				}
			case <-ctx.Done():
				return
			}
		}
	}()
	return out, nil
}

func (u *OuService) logCall(ctx context.Context, method string, fields map[string]any, err error) {
	args := []any{"method", method}
	for k, v := range fields {
		args = append(args, k, v)
	}
	if err != nil {
		args = append(args, "error", err)
		u.log.ErrorContext(ctx, "ou Service call failed", args...)
		return
	}
	u.log.InfoContext(ctx, "ou Service call ok", args...)
}
