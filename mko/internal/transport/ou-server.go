package transport

// Файл реализации интерфейсов

import (
	"context"
	"errors"
	mkoapiv1 "mko/api/gen"
	"mko/internal/domain"

	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

type OuServiceInterface interface {
	GetVersion(ctx context.Context) (string, error)
	Ping(ctx context.Context) error

	ConfigureOu(ctx context.Context, cfg domain.OuConfig) error
	SetOuResponseWord(ctx context.Context, board domain.BoardID, index domain.McoIndex, channel domain.Channel, ouAddress int32, word int32) error
	ReadOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, receiveArea bool) (domain.OuSubaddressData, error)
	WriteOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, sd []uint32) error
	SubscribeOuCommands(ctx context.Context, board domain.BoardID) (<-chan domain.OuCommandEvent, error)
}

func (m *MKOServer) GetStatus(ctx context.Context, _ *mkoapiv1.Empty) (*mkoapiv1.StatusResponse, error) {
	version, err := m.ouService.GetVersion(ctx)
	if err != nil {
		return &mkoapiv1.StatusResponse{DriverConnected: false}, nil // health-check: не ошибка API, а статус
	}
	pingErr := m.ouService.Ping(ctx)
	return &mkoapiv1.StatusResponse{
		ServiceVersion:  "0.1.0",
		DriverConnected: pingErr == nil,
		DriverVersion:   version,
	}, nil
}

func (m *MKOServer) ConfigureOu(ctx context.Context, request *mkoapiv1.ConfigureOuRequest) (*mkoapiv1.Empty, error) {
	cfg := domain.OuConfig{
		Board:        domain.BoardID(request.GetBoardId()),
		Index:        domain.McoIndex(request.GetIndex()),
		Channel:      domain.Channel(request.GetChannel()),
		OuAddress:    request.GetOuAddress(),
		ResponseWord: request.GetResponseWord(),
		RemoteIP:     request.GetRemoteIp(),
		RemotePort:   request.GetRemotePort(),
	}
	if err := validateOuAddress(cfg.OuAddress); err != nil {
		return nil, toGRPCStatus(err)
	}
	if err := m.ouService.ConfigureOu(ctx, cfg); err != nil {
		return nil, toGRPCStatus(err)
	}
	return &mkoapiv1.Empty{}, nil
}

func (m *MKOServer) SetOuResponseWord(ctx context.Context, request *mkoapiv1.SetOuResponseWordRequest) (*mkoapiv1.Empty, error) {
	err := m.ouService.SetOuResponseWord(
		ctx,
		domain.BoardID(request.GetBoardId()),
		domain.McoIndex(request.GetIndex()),
		domain.Channel(request.GetChannel()),
		request.GetOuAddress(),
		request.GetResponseWord(),
	)
	if err != nil {
		return nil, toGRPCStatus(err)
	}
	return &mkoapiv1.Empty{}, nil
}

func (m *MKOServer) ReadOuSubaddress(ctx context.Context, request *mkoapiv1.ReadOuSubaddressRequest) (*mkoapiv1.OuSubaddressData, error) {
	if err := validateSubaddress(request.GetSubaddress()); err != nil {
		return nil, toGRPCStatus(err)
	}
	data, err := m.ouService.ReadOuSubaddress(ctx, domain.BoardID(request.GetBoardId()), request.GetSubaddress(), request.GetReceiveArea())
	if err != nil {
		return nil, toGRPCStatus(err)
	}
	return &mkoapiv1.OuSubaddressData{
		Subaddress:    data.Subaddress,
		Sd:            data.Sd,
		CmdWord:       data.CmdWord,
		ResultWord:    data.ResultWord,
		DecodedResult: data.DecodedResult,
	}, nil
}

func (m *MKOServer) WriteOuSubaddress(ctx context.Context, request *mkoapiv1.WriteOuSubaddressRequest) (*mkoapiv1.Empty, error) {
	if err := validateSubaddress(request.GetSubaddress()); err != nil {
		return nil, toGRPCStatus(err)
	}
	if err := validateWriteWords(request.GetSd()); err != nil {
		return nil, toGRPCStatus(err)
	}
	err := m.ouService.WriteOuSubaddress(ctx, domain.BoardID(request.GetBoardId()), request.GetSubaddress(), request.GetSd())
	if err != nil {
		return nil, toGRPCStatus(err)
	}
	return &mkoapiv1.Empty{}, nil
}

// SendRawOuData — UC-8: намеренно БЕЗ вызова validateSubaddress/
// validateWriteWords. Всё, что вернётся, — это реальный ответ
// драйвера/платы, а не ваша валидация.
func (m *MKOServer) SendRawOuData(ctx context.Context, request *mkoapiv1.WriteOuSubaddressRequest) (*mkoapiv1.Empty, error) {
	err := m.ouService.WriteOuSubaddress(ctx, domain.BoardID(request.GetBoardId()), request.GetSubaddress(), request.GetSd())
	if err != nil {
		return nil, toGRPCStatus(err)
	}
	return &mkoapiv1.Empty{}, nil
}

func (m *MKOServer) SubscribeOuCommands(request *mkoapiv1.SubscribeOuCommandsRequest, g grpc.ServerStreamingServer[mkoapiv1.OuCommandEvent]) error {
	ctx := g.Context()
	board := domain.BoardID(request.GetBoardId())

	events, err := m.ouService.SubscribeOuCommands(ctx, board)
	if err != nil {
		return toGRPCStatus(err)
	}

	for {
		select {
		case ev, ok := <-events:
			if !ok {
				return nil
			}
			if sendErr := g.Send(toProtoOuCommandEvent(ev)); sendErr != nil {
				return sendErr
			}
		case <-ctx.Done():
			return ctx.Err()
		}
	}
}

func (m *MKOServer) ClearReceiveBuffer(ctx context.Context, request *mkoapiv1.ClearBufferRequest) (*mkoapiv1.Empty, error) {
	return nil, toGRPCStatus(domain.WrapNotImplemented("TCR! is not exposed by mko_driver yet"))
}

func (m *MKOServer) ClearTransmitBuffer(ctx context.Context, request *mkoapiv1.ClearBufferRequest) (*mkoapiv1.Empty, error) {
	return nil, toGRPCStatus(domain.WrapNotImplemented("TCT! is not exposed by mko_driver yet"))
}

// ============================================================
// Валидация (ваша сторона, драйвер это не гарантирует)
// ============================================================

func validateSubaddress(sub uint32) error {
	if sub > 30 {
		return domain.NewValidationError("subaddress", "must be in range 0..30")
	}
	return nil
}

func validateOuAddress(addr int32) error {
	if addr < 0 || addr > 30 {
		return domain.NewValidationError("ou_address", "must be in range 0..30")
	}
	return nil
}

func validateWriteWords(sd []uint32) error {
	if len(sd) < 1 || len(sd) > 32 {
		return domain.NewValidationError("sd", "word count must be in range 1..32")
	}
	return nil
}

func toProtoOuCommandEvent(ev domain.OuCommandEvent) *mkoapiv1.OuCommandEvent {
	return &mkoapiv1.OuCommandEvent{
		CmdWord:        ev.CmdWord,
		ResultWord:     ev.ResultWord,
		ReceiveFromOu:  ev.ReceiveFromOu,
		OuAddress:      ev.OuAddress,
		Subaddress:     ev.Subaddress,
		WordCount:      ev.WordCount,
		DecodedCommand: ev.DecodedCommand,
		DecodedResult:  ev.DecodedResult,
	}
}

func toGRPCStatus(err error) error {
	if err == nil {
		return nil
	}
	switch {
	case errors.Is(err, domain.ErrValidation):
		return status.Error(codes.InvalidArgument, err.Error())
	case errors.Is(err, domain.ErrDeviceRejected):
		return status.Error(codes.FailedPrecondition, err.Error())
	case errors.Is(err, domain.ErrTimeout):
		return status.Error(codes.DeadlineExceeded, err.Error())
	case errors.Is(err, domain.ErrDriverUnavailable):
		return status.Error(codes.Unavailable, err.Error())
	case errors.Is(err, domain.ErrNotImplemented):
		return status.Error(codes.Unimplemented, err.Error())
	default:
		return status.Error(codes.Internal, err.Error())
	}
}
