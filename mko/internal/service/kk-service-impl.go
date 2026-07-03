package service

import (
	"context"
	"mko/internal/domain"
)

type kkService struct {
	driver KKDriver
}

func NewKKService(driver KKDriver) KKService {
	return &kkService{driver: driver}
}

func (s *kkService) ConfigureKK(ctx context.Context, cfg domain.KKConfig) error {
	return s.driver.ConfigureMko(ctx, cfg)
}

func (s *kkService) ConfigureExchange(ctx context.Context, cfg domain.ExchangeConfig) error {
	return s.driver.ConfigureExchange(ctx, cfg)
}

func (s *kkService) RunExchange(ctx context.Context, cmd domain.RunExchangeCommand) error {
	return s.driver.SendCommandRun(ctx, cmd)
}

func (s *kkService) SubscribeExchangeResults(ctx context.Context, boardID string) (<-chan domain.ExchangeResult, error) {
	return s.driver.SubscribeExchangeResults(ctx, boardID)
}
