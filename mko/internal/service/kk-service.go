package service

import (
	"context"
	"errors"
	"fmt"
	"time"

	"mko/internal/domain"
)

const defaultKKCommandTimeout = 1500 * time.Millisecond

type KKService interface {
	ConfigureKK(ctx context.Context, cfg domain.KKConfig) error
	ConfigureExchange(ctx context.Context, cfg domain.ExchangeConfig) error
	RunExchange(ctx context.Context, cmd domain.RunExchangeCommand) error
	SubscribeExchangeResults(ctx context.Context, boardID string) (<-chan domain.ExchangeResult, error)
}

type KKDriver interface {
	ConfigureMko(ctx context.Context, cfg domain.KKConfig) error
	ConfigureExchange(ctx context.Context, cfg domain.ExchangeConfig) error
	SendCommandRun(ctx context.Context, cmd domain.RunExchangeCommand) error
	SubscribeExchangeResults(ctx context.Context, boardID string) (<-chan domain.ExchangeResult, error)
}

type kkService struct {
	driver         KKDriver
	commandTimeout time.Duration
}

func NewKKService(driver KKDriver) KKService {
	return &kkService{
		driver:         driver,
		commandTimeout: defaultKKCommandTimeout,
	}
}

func (s *kkService) ConfigureKK(ctx context.Context, cfg domain.KKConfig) error {
	ctx, cancel := context.WithTimeout(ctx, s.commandTimeout)
	defer cancel()

	if err := s.driver.ConfigureMko(ctx, cfg); err != nil {
		return mapKKDriverError(ctx, "configure kk", cfg.OperationID, string(cfg.Board), err)
	}

	return nil
}

func (s *kkService) ConfigureExchange(ctx context.Context, cfg domain.ExchangeConfig) error {
	ctx, cancel := context.WithTimeout(ctx, s.commandTimeout)
	defer cancel()

	if err := s.driver.ConfigureExchange(ctx, cfg); err != nil {
		return mapKKDriverError(ctx, "configure exchange", cfg.OperationID, string(cfg.Board), err)
	}

	return nil
}

func (s *kkService) RunExchange(ctx context.Context, cmd domain.RunExchangeCommand) error {
	ctx, cancel := context.WithTimeout(ctx, s.commandTimeout)
	defer cancel()

	if err := s.driver.SendCommandRun(ctx, cmd); err != nil {
		return mapKKDriverError(ctx, "run exchange", cmd.OperationID, string(cmd.Board), err)
	}

	return nil
}

func (s *kkService) SubscribeExchangeResults(ctx context.Context, boardID string) (<-chan domain.ExchangeResult, error) {
	ch, err := s.driver.SubscribeExchangeResults(ctx, boardID)
	if err != nil {
		return nil, mapKKDriverError(ctx, "subscribe exchange results", "", boardID, err)
	}

	return ch, nil
}

func mapKKDriverError(ctx context.Context, operation, operationID, boardID string, err error) error {
	if err == nil {
		return nil
	}

	details := fmt.Sprintf("%s board_id=%s", operation, boardID)
	if operationID != "" {
		details = fmt.Sprintf("%s operation_id=%s", details, operationID)
	}

	if errors.Is(ctx.Err(), context.DeadlineExceeded) || errors.Is(err, context.DeadlineExceeded) {
		return fmt.Errorf("%w: %s", domain.ErrTimeout, details)
	}

	if errors.Is(ctx.Err(), context.Canceled) || errors.Is(err, context.Canceled) {
		return fmt.Errorf("%w: %s", domain.ErrOperationCanceled, details)
	}

	return fmt.Errorf("%w: %s: %v", domain.ErrDeviceRejected, details, err)
}
