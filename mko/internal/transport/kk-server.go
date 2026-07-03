package transport

import (
	"context"
	"mko/internal/domain"
)

type KKServiceInterface interface {
	ConfigureKK(ctx context.Context, cfg domain.KKConfig) error
	ConfigureExchange(ctx context.Context, cfg domain.ExchangeConfig) error
	RunExchange(ctx context.Context, cmd domain.RunExchangeCommand) error
	SubscribeExchangeResults(ctx context.Context, boardID string) (<-chan domain.ExchangeResult, error)
}

func (s *MKOServer) ConfigureMko(ctx context.Context, req *mkoapiv1.MkoConfigRequest) (*mkoapiv1.StandardResponse, error) {
	err := s.kkServer.ConfigureKK(ctx, domain.KKConfig{
		Index:        domain.McoIndex(req.Index),
		Channel:      domain.Channel(req.Channel),
		BusControl:   domain.BusControl(req.BusControl),
		OUAddress:    int(req.OuAddress),
		OURespWord:   uint32(req.OuRespWord),
		VectorWord:   domain.Ve(req.VectorWord),
		SelftestWord: uint32(req.SelftestWord),
		RemoteIP:     req.RemoteIp,
		RemotePort:   int(req.RemotePort),
		OperationID:  req.OperationId,
		BoardID:      req.BoardId,
	})
	if err != nil {
		return failResponse(err), nil
	}

	return okResponse(), nil
}

func (s *MKOServer) ConfigureExcnage(ctx context.Context, req *mkoapiv1.ExchangeConfigRequest) (*mkoapiv1.StandardResponse, error) {
	err := s.kkServer.ConfigureExchange(ctx, domain.ExchangeConfig{
		Format:      req.Format,
		KS1:         req.Ks1,
		KS2:         req.Ks2,
		SD:          req.Sd,
		OperationID: req.OperationId,
		BoardID:     req.BoardId,
	})
	if err != nil {
		return failResponse(err), nil
	}
	return okResponse(), nil
}

func (s *MKOServer) SendCommandRun(ctx context.Context, req *mkoapiv1.ActionRequest) (*mkoapiv1.StandardResponse, error) {
	err := s.kkServer.RunExchange(ctx, domain.RunExchangeCommand{
		OperationID: req.OperationId,
		BoardID:     req.BoardId,
	})
	if err != nil {
		return failResponse(err), nil
	}
	return okResponse(), nil
}

func (s *MKOServer) SubscribeExchangeResultsForBoard(
	req *mkoapiv1.BoardRequest,
	stream mkoapiv1.MkoWorkstationService_SubscribeExchangeResultsForBoardServer,
) error {
	results, err := s.kkServer.SubscribeExchangeResults(stream.Context(), req.BoardId)
	if err != nil {
		return err
	}
	for result := range results {
		if err := stream.Send(&mkoapiv1.ExchangeResultResponse{
			Format:        result.Format,
			Ks1:           result.KS1,
			Ks2:           result.KS2,
			Sd:            result.SD,
			AnswerWord_1:  result.AnswerWord1,
			AnswerWord_2:  result.AnswerWord2,
			DecodedResult: result.DecodeResult,
		}); err != nil {
			return err
		}
	}
	return nil
}

func okResponse() *mkoapiv1.StandardResponse {
	return &mkoapiv1.StandardResponse{
		Success: true,
	}
}

func failResponse(err error) *mkoapiv1.StandardResponse {
	return &mkoapiv1.StandardResponse{
		Success:      false,
		ErrorMessage: err.Error(),
	}
}
