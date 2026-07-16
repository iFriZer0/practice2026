package mko_driver_service

import (
	"context"
	"errors"
	"fmt"
	"time"

	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
	"google.golang.org/protobuf/types/known/emptypb"

	"mko/internal/domain"
	mkodriverv1 "mko/internal/transport/clients/mko-driver-service/gen/mkodriverv1"
)

// unaryTimeout > 1500 мс, которые ждёт сам драйвер внутри себя
// (см. документацию mko_driver, раздел про таймауты) — иначе вы
// отвалитесь раньше, чем драйвер успеет честно дождаться платы.
const unaryTimeout = 2 * time.Second

type Client struct {
	grpc mkodriverv1.MkoDriverClient
}

func New(grpcClient mkodriverv1.MkoDriverClient) *Client {
	return &Client{grpc: grpcClient}
}

func (c *Client) GetVersion(ctx context.Context) (string, error) {
	ctx, cancel := context.WithTimeout(ctx, unaryTimeout)
	defer cancel()

	resp, err := c.grpc.GetVersion(ctx, &emptypb.Empty{})
	if err != nil {
		return "", mapDriverError(err)
	}
	return resp.GetVersion(), nil
}

// Ping — health-check связи с платой. ВАЖНО: ReadMemory в текущем
// HEAD драйвера игнорирует address/size и реально просто проверяет
// доступность платы (см. документацию, п.5.3) — не пытайтесь через
// него читать/писать что-то содержательное.
func (c *Client) Ping(ctx context.Context) error {
	ctx, cancel := context.WithTimeout(ctx, unaryTimeout)
	defer cancel()

	_, err := c.grpc.ReadMemory(ctx, &mkodriverv1.ReadMemoryRequest{
		OperationId: newOperationID(),
	})
	return mapDriverError(err)
}

func (c *Client) ConfigureOu(ctx context.Context, cfg domain.OuConfig) error {
	ctx, cancel := context.WithTimeout(ctx, unaryTimeout)
	defer cancel()

	resp, err := c.grpc.ConfigureOu(ctx, &mkodriverv1.ConfigureOuRequest{
		Index:        int32(cfg.Index),
		Channel:      int32(cfg.Channel),
		OuAddress:    cfg.OuAddress,
		ResponseWord: cfg.ResponseWord,
		RemoteIp:     cfg.RemoteIP,
		RemotePort:   cfg.RemotePort,
		BoardId:      string(cfg.Board),
		OperationId:  newOperationID(),
	})
	if err != nil {
		return mapDriverError(err)
	}
	// ВАЖНО: драйвер после конфигурации сам делает readMainInfo() и
	// при недоступности платы возвращает success=false БЕЗ grpc-ошибки
	// (см. документацию mko_driver, раздел ConfigureMko/ConfigureOu).
	// Проверка только err здесь недостаточна — это и была причина бага.
	if !resp.GetSuccess() {
		return domain.WrapDeviceRejected(resp.GetErrorMessage())
	}
	return nil
}

func (c *Client) SetOuResponseWord(ctx context.Context, board domain.BoardID, index domain.McoIndex, channel domain.Channel, ouAddress int32, word int32) error {
	ctx, cancel := context.WithTimeout(ctx, unaryTimeout)
	defer cancel()

	resp, err := c.grpc.SetOuResponseWord(ctx, &mkodriverv1.SetOuResponseWordRequest{
		Index:        int32(index),
		Channel:      int32(channel),
		OuAddress:    ouAddress,
		ResponseWord: word,
		BoardId:      string(board),
		OperationId:  newOperationID(),
	})
	if err != nil {
		return mapDriverError(err)
	}
	if !resp.GetSuccess() {
		return domain.WrapDeviceRejected(resp.GetErrorMessage())
	}
	return nil
}

func (c *Client) ReadOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, receiveArea bool) (domain.OuSubaddressData, error) {
	ctx, cancel := context.WithTimeout(ctx, unaryTimeout)
	defer cancel()

	resp, err := c.grpc.ReadOuSubaddress(ctx, &mkodriverv1.ReadOuSubaddressRequest{
		Subaddress:  sub,
		ReceiveArea: receiveArea,
		BoardId:     string(board),
		OperationId: newOperationID(),
	})
	if err != nil {
		return domain.OuSubaddressData{}, mapDriverError(err)
	}
	if !resp.GetSuccess() {
		return domain.OuSubaddressData{}, domain.WrapDeviceRejected(resp.GetErrorMessage())
	}
	return domain.OuSubaddressData{
		Board:         board,
		Subaddress:    resp.GetSubaddress(),
		Area:          domain.OuArea(receiveArea),
		Sd:            resp.GetSd(),
		CmdWord:       resp.GetCmdWord(),
		ResultWord:    resp.GetResultWord(),
		DecodedResult: resp.GetDecodedResult(),
	}, nil
}

func (c *Client) WriteOuSubaddress(ctx context.Context, board domain.BoardID, sub uint32, sd []uint32) error {
	ctx, cancel := context.WithTimeout(ctx, unaryTimeout)
	defer cancel()

	resp, err := c.grpc.WriteOuSubaddress(ctx, &mkodriverv1.WriteOuSubaddressRequest{
		Subaddress:  sub,
		Sd:          sd,
		BoardId:     string(board),
		OperationId: newOperationID(),
	})
	if err != nil {
		return mapDriverError(err)
	}
	if !resp.GetSuccess() {
		return domain.WrapDeviceRejected(resp.GetErrorMessage())
	}
	return nil
}

// SubscribeOuCommands открывает долгоживущий стрим к драйверу и
// форвардит события в канал domain.OuCommandEvent.
func (c *Client) SubscribeOuCommands(ctx context.Context, board domain.BoardID) (<-chan domain.OuCommandEvent, error) {
	stream, err := c.grpc.SubscribeOuCommandsForBoard(ctx, &mkodriverv1.BoardRequest{
		BoardId: string(board),
	})
	if err != nil {
		return nil, mapDriverError(err)
	}

	out := make(chan domain.OuCommandEvent)
	go func() {
		defer close(out)
		for {
			ev, err := stream.Recv()
			if err != nil {
				return
			}
			select {
			case out <- domain.OuCommandEvent{
				Board:          board,
				CmdWord:        ev.GetCmdWord(),
				ResultWord:     ev.GetResultWord(),
				ReceiveFromOu:  ev.GetReceiveFromOu(),
				OuAddress:      ev.GetOuAddress(),
				Subaddress:     ev.GetSubaddress(),
				WordCount:      ev.GetWordCount(),
				DecodedCommand: ev.GetDecodedCommand(),
				DecodedResult:  ev.GetDecodedResult(),
				ReceivedAt:     time.Now(),
			}:
			case <-ctx.Done():
				return
			}
		}
	}()
	return out, nil
}

func mapDriverError(err error) error {
	if err == nil {
		return nil
	}
	st, ok := status.FromError(err)
	if !ok {
		return err
	}
	switch st.Code() {
	case codes.DeadlineExceeded:
		return domain.WrapTimeout(st.Message())
	case codes.Unavailable:
		return domain.WrapDriverUnavailable(st.Message())
	case codes.Unimplemented:
		return domain.WrapNotImplemented(st.Message())
	case codes.InvalidArgument, codes.FailedPrecondition:
		return domain.WrapDeviceRejected(st.Message())
	default:
		return errors.New(st.Message())
	}
}

func newOperationID() string {
	return fmt.Sprintf("op-%d", time.Now().UnixNano())
}
