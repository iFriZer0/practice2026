package transport

import (
	"context"
	"fmt"
	"io"

	"mko/internal/domain"
	"mko/internal/service"
	mkodriverv1 "mko/internal/transport/clients/mko-driver-service/gen/mkodriverv1"
)

type MKODriverClient struct {
	client mkodriverv1.MkoDriverClient
}

var _ service.KKDriver = (*MKODriverClient)(nil)

func NewMKODriverClient(client mkodriverv1.MkoDriverClient) *MKODriverClient {
	return &MKODriverClient{
		client: client,
	}
}

func (c *MKODriverClient) ConfigureMko(ctx context.Context, cfg domain.KKConfig) error {
	resp, err := c.client.ConfigureMko(ctx, &mkodriverv1.MkoConfigRequest{
		Index:        int32(cfg.Index),
		Mode:         int32(domain.ModeKk),
		Channel:      int32(cfg.Channel),
		BusControl:   int32(cfg.BusControl),
		OuAddress:    cfg.OuAddress,
		OuRespWord:   cfg.OuRespWord,
		VectorWord:   cfg.VectorWord,
		SelftestWord: cfg.SelftestWord,
		RemoteIp:     cfg.RemoteIP,
		RemotePort:   cfg.RemotePort,
		OperationId:  cfg.OperationID,
		BoardId:      string(cfg.Board),
	})
	if err != nil {
		return err
	}
	return standardResponseError(resp)
}

func (c *MKODriverClient) ConfigureExchange(ctx context.Context, cfg domain.ExchangeConfig) error {
	resp, err := c.client.ConfigureExchange(ctx, &mkodriverv1.ExchangeConfigRequest{
		Format:      cfg.Format,
		Ks1:         cfg.KS1,
		Ks2:         cfg.KS2,
		Sd:          cfg.SD,
		OperationId: cfg.OperationID,
		BoardId:     string(cfg.Board),
	})
	if err != nil {
		return err
	}
	return standardResponseError(resp)
}

func (c *MKODriverClient) SendCommandRun(ctx context.Context, cmd domain.RunExchangeCommand) error {
	resp, err := c.client.SendCommandRun(ctx, &mkodriverv1.ActionRequest{
		OperationId: cmd.OperationID,
		BoardId:     string(cmd.Board),
	})
	if err != nil {
		return err
	}
	return standardResponseError(resp)
}

func (c *MKODriverClient) SubscribeExchangeResults(ctx context.Context, boardID string) (<-chan domain.ExchangeResult, error) {
	stream, err := c.client.SubscribeExchangeResultsForBoard(ctx, &mkodriverv1.BoardRequest{
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
				Board:         domain.BoardID(boardID),
				Format:        msg.GetFormat(),
				KS1:           msg.GetKs1(),
				KS2:           msg.GetKs2(),
				SD:            msg.GetSd(),
				AnswerWord1:   msg.GetAnswerWord_1(),
				AnswerWord2:   msg.GetAnswerWord_2(),
				ResultWord:    msg.GetResultWord(),
				DecodedResult: msg.GetDecodedResult(),
			}
		}
	}()
	return results, nil
}

func standardResponseError(resp *mkodriverv1.StandardResponse) error {
	if resp == nil {
		return fmt.Errorf("%w: empty driver response", domain.ErrDriverUnavailable)
	}
	if resp.GetSuccess() {
		return nil
	}
	return fmt.Errorf("%w: %s", domain.ErrDeviceRejected, resp.GetErrorMessage())
}
