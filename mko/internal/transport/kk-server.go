package transport

import (
	"context"

	mkoapiv1 "mko/api/gen"
	"mko/internal/domain"

	"google.golang.org/grpc"
)

type KKServiceInterface interface {
	ConfigureKK(ctx context.Context, cfg domain.KKConfig) error
	ConfigureExchange(ctx context.Context, cfg domain.ExchangeConfig) error
	RunExchange(ctx context.Context, cmd domain.RunExchangeCommand) error
	SubscribeExchangeResults(ctx context.Context, boardID string) (<-chan domain.ExchangeResult, error)
}

func (m *MKOServer) ConfigureKK(ctx context.Context, req *mkoapiv1.ConfigureKKRequest) (*mkoapiv1.Empty, error) {
	cfg := domain.KKConfig{
		Board:        domain.BoardID(req.GetBoardId()),
		Index:        domain.McoIndex(req.GetIndex()),
		Channel:      domain.Channel(req.GetChannel()),
		BusControl:   domain.BusControl(req.GetBusControl()),
		OuAddress:    req.GetOuAddress(),
		OuRespWord:   req.GetOuRespWord(),
		VectorWord:   req.GetVectorWord(),
		SelftestWord: req.GetSelftestWord(),
		RemoteIP:     req.GetRemoteIp(),
		RemotePort:   req.GetRemotePort(),
		OperationID:  req.GetOperationId(),
	}

	if err := m.kkService.ConfigureKK(ctx, cfg); err != nil {
		return nil, toGRPCStatus(err)
	}
	return &mkoapiv1.Empty{}, nil
}

func (m *MKOServer) ConfigureExchange(ctx context.Context, req *mkoapiv1.ConfigureExchangeRequest) (*mkoapiv1.Empty, error) {
	cfg := domain.ExchangeConfig{
		Board:       domain.BoardID(req.GetBoardId()),
		Format:      req.GetFormat(),
		KS1:         req.GetKs1(),
		KS2:         req.GetKs2(),
		SD:          req.GetSd(),
		OperationID: req.GetOperationId(),
	}

	if err := m.kkService.ConfigureExchange(ctx, cfg); err != nil {
		return nil, toGRPCStatus(err)
	}
	return &mkoapiv1.Empty{}, nil
}

func (m *MKOServer) RunExchange(ctx context.Context, req *mkoapiv1.RunExchangeRequest) (*mkoapiv1.Empty, error) {
	cfg := domain.RunExchangeCommand{
		Board:       domain.BoardID(req.GetBoardId()),
		OperationID: req.GetOperationId(),
	}

	if err := m.kkService.RunExchange(ctx, cfg); err != nil {
		return nil, toGRPCStatus(err)
	}
	return &mkoapiv1.Empty{}, nil
}

func (m *MKOServer) SubscribeExchangeResults(
	req *mkoapiv1.SubscribeExchangeResultsRequest,
	stream grpc.ServerStreamingServer[mkoapiv1.ExchangeResult],
) error {
	results, err := m.kkService.SubscribeExchangeResults(stream.Context(), req.GetBoardId())
	if err != nil {
		return toGRPCStatus(err)
	}

	for result := range results {
		if err := stream.Send(&mkoapiv1.ExchangeResult{
			Format:        result.Format,
			Ks1:           result.KS1,
			Ks2:           result.KS2,
			Sd:            result.SD,
			AnswerWord_1:  result.AnswerWord1,
			AnswerWord_2:  result.AnswerWord2,
			ResultWord:    result.ResultWord,
			DecodedResult: result.DecodedResult,
		}); err != nil {
			return err
		}
	}
	return nil
}
