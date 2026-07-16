package service

import (
	"context"

	"mko/internal/domain"
)

// DriverOuClient — клиент mko_driver для операций в режиме ОУ.
type DriverOuClient interface {
	GetVersion(ctx context.Context) (string, error)
	Ping(ctx context.Context) error

	ConfigureOu(ctx context.Context, cfg domain.OuConfig) error
	SetOuResponseWord(ctx context.Context, board domain.BoardID, index domain.McoIndex, channel domain.Channel, ouAddress int32, word int32) error
	ReadOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, receiveArea bool) (domain.OuSubaddressData, error)
	WriteOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, sd []uint32) error
	SubscribeOuCommands(ctx context.Context, board domain.BoardID) (<-chan domain.OuCommandEvent, error)
}

// OuService — бизнес-логика режима ОУ. Реализация НЕ занимается
// логированием (см. internal/service/kk-service.go — тот же принцип):
// логирование вынесено в отдельный декоратор internal/logging.NewOuService,
// который оборачивает это же значение интерфейса. Так бизнес-логика
// остаётся тестируемой и не размазывает логирование по коду.
type OuService interface {
	GetVersion(ctx context.Context) (string, error)
	Ping(ctx context.Context) error

	ConfigureOu(ctx context.Context, cfg domain.OuConfig) error
	SetOuResponseWord(ctx context.Context, board domain.BoardID, index domain.McoIndex, channel domain.Channel, ouAddress int32, word int32) error
	ReadOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, receiveArea bool) (domain.OuSubaddressData, error)
	WriteOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, sd []uint32) error
	SubscribeOuCommands(ctx context.Context, board domain.BoardID) (<-chan domain.OuCommandEvent, error)
}

type ouService struct {
	driver DriverOuClient
}

func NewOuService(driver DriverOuClient) OuService {
	return &ouService{driver: driver}
}

func (u *ouService) GetVersion(ctx context.Context) (string, error) {
	return u.driver.GetVersion(ctx)
}

func (u *ouService) Ping(ctx context.Context) error {
	return u.driver.Ping(ctx)
}

// ConfigureOu нормализует index по правилам протокола МКО: драйвер
// сам приводит index<=0 к 1 (см. документацию mko_driver, п.5.2), но
// делает это молча внутри себя. Явно повторяем это правило здесь, а
// не полагаемся на скрытое поведение драйвера — так поведение
// тестируемо на уровне бизнес-логики. Кто вызвал с index<=0 и получил
// index=1, виден в логах декоратора: он логирует и cfg.Index на
// входе, и фактически применённый cfg после нормализации.
func (u *ouService) ConfigureOu(ctx context.Context, cfg domain.OuConfig) error {
	if cfg.Index <= 0 {
		cfg.Index = 1
	}
	return u.driver.ConfigureOu(ctx, cfg)
}

func (u *ouService) SetOuResponseWord(ctx context.Context, board domain.BoardID, index domain.McoIndex, channel domain.Channel, ouAddress int32, word int32) error {
	if index <= 0 {
		index = 1
	}
	return u.driver.SetOuResponseWord(ctx, board, index, channel, ouAddress, word)
}

func (u *ouService) ReadOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, receiveArea bool) (domain.OuSubaddressData, error) {
	return u.driver.ReadOuSubaddress(ctx, board, sub, receiveArea)
}

func (u *ouService) WriteOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, sd []uint32) error {
	return u.driver.WriteOuSubaddress(ctx, board, sub, sd)
}

// SubscribeOuCommands прокидывает канал от driver-клиента как есть.
// Логирование каждого события — забота декоратора
// internal/logging.NewOuService, а не этого слоя.
func (u *ouService) SubscribeOuCommands(ctx context.Context, board domain.BoardID) (<-chan domain.OuCommandEvent, error) {
	return u.driver.SubscribeOuCommands(ctx, board)
}
