package service

import (
	"context"
	"mko/internal/domain"
)

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
