package transport

import (
	"context"
	"io"
	"mko/internal/domain"
	"mko/internal/service"
)

type MKODriverClient struct {
	client mkoapiv1.MkoDriverClient
}

var _ service.KKDriver = (*MKODriverClient)(nil)

func NewMKODriverClient(client mkoapiv1.MkoDriverClient) *MKODriverClient {
	return &MKODriverClient{
		client: client,
	}
}

func (c *MKODriverClient) ConfigureMko(ctx context.Context, cfg domain.KKConfig) error {
	_, err := c.client.ConfigureMko(ctx, &mkoapiv1.MkoConfigRequest{
		Index:        int32(cfg.Index),
		Mode:         1,
		Channel:      int32(cfg.Channel),
		BusControl:   int32(cfg.BusControl),
		OuAddress:    int32(cfg.OUAddress),
		OuRespWord:   int32(cfg.OURespWord),
		VectorWord:   int32(cfg.VectorWord),
		SelftestWord: int32(cfg.SelftestWord),
		RemoteIp:     cfg.RemoteIP,
		RemotePort:   int32(cfg.RemotePort),
		OperationId:  cfg.OperationID,
		BoardId:      cfg.BoardID,
	})
	return err
}

func (c *MKODriverClient) ConfigureExchange(ctx context.Context, cfg domain.ExchangeConfig) error {
	_, err := c.client.ConfigureExchange(ctx, &mkoapiv1.ExchangeConfigRequest{
		Format:      cfg.Format,
		Ks1:         cfg.KS1,
		Ks2:         cfg.KS2,
		Sd:          cfg.SD,
		OperationId: cfg.OperationID,
		BoardId:     cfg.BoardID,
	})
	return err
}

func (c *MKODriverClient) SendCommandRun(ctx context.Context, cmd domain.RunExchangeCommand) error {
	_, err := c.client.SendCommandRun(ctx, &mkoapiv1.ActionRequest{
		OperationId: cmd.OperationID,
		BoardId:     cmd.BoardID,
	})
	return err
}

func (c *MKODriverClient) SubscribeExchangeResults(ctx context.Context, boardID string) (<-chan domain.ExchangeResult, error) {
	stream, err := c.client.SubscribeExchangeResultsForBoard(ctx, &mkoapiv1.BoardRequest{
		BoardId: boardID,
	})
	if err != nil {
		return nil, err
	}
	results := make(chan domain.ExchangeResult)

	go func() {
		defer close(results)

		for {
			msg, err := stream.Recv()
			if err == io.EOF {
				return
			}
			if err != nil {
				return
			}

			results <- domain.ExchangeResult{
				Format:       msg.Format,
				KS1:          msg.Ks1,
				KS2:          msg.Ks2,
				SD:           msg.Sd,
				AnswerWord1:  msg.AnswerWord_1,
				AnswerWord2:  msg.AnswerWord_2,
				DecodeResult: msg.DecodedResult,
			}
		}
	}()
	return results, nil
}
