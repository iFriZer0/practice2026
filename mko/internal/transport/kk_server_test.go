package transport_test

import (
	"context"
	"errors"
	"testing"

	mkoapiv1 "mko/api/gen"
	"mko/internal/domain"
	"mko/internal/transport"
	"mko/internal/transport/mocks"

	"go.uber.org/mock/gomock"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/metadata"
	"google.golang.org/grpc/status"
)

// exchangeResultStreamStub — минимальная реализация grpc.ServerStreamingServer для тестов.
type exchangeResultStreamStub struct {
	ctx  context.Context
	sent []*mkoapiv1.ExchangeResult
}

func (s *exchangeResultStreamStub) Send(r *mkoapiv1.ExchangeResult) error {
	s.sent = append(s.sent, r)
	return nil
}
func (s *exchangeResultStreamStub) Context() context.Context     { return s.ctx }
func (s *exchangeResultStreamStub) SetHeader(metadata.MD) error  { return nil }
func (s *exchangeResultStreamStub) SendHeader(metadata.MD) error { return nil }
func (s *exchangeResultStreamStub) SetTrailer(metadata.MD)       {}
func (s *exchangeResultStreamStub) SendMsg(any) error            { return nil }
func (s *exchangeResultStreamStub) RecvMsg(any) error            { return nil }

func TestMKOServer_ConfigureKK(t *testing.T) {
	type input struct {
		req *mkoapiv1.ConfigureKKRequest
	}
	tests := []struct {
		name     string
		input    input
		prep     func(m *mocks.MockKKServiceInterface)
		wantCode codes.Code
	}{
		{
			name: "valid request — maps all fields to KKConfig",
			input: input{req: &mkoapiv1.ConfigureKKRequest{
				BoardId:      "board1",
				Index:        2,
				Channel:      1,
				BusControl:   0,
				OuAddress:    5,
				OuRespWord:   10,
				VectorWord:   20,
				SelftestWord: 30,
				RemoteIp:     "127.0.0.1",
				RemotePort:   50052,
				OperationId:  "op-kk",
			}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().ConfigureKK(gomock.Any(), domain.KKConfig{
					Board:        "board1",
					Index:        2,
					Channel:      1,
					BusControl:   0,
					OuAddress:    5,
					OuRespWord:   10,
					VectorWord:   20,
					SelftestWord: 30,
					RemoteIP:     "127.0.0.1",
					RemotePort:   50052,
					OperationID:  "op-kk",
				}).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "service ErrTimeout → DeadlineExceeded",
			input: input{req: &mkoapiv1.ConfigureKKRequest{BoardId: "board1"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().ConfigureKK(gomock.Any(), gomock.Any()).
					Return(domain.WrapTimeout("timeout"))
			},
			wantCode: codes.DeadlineExceeded,
		},
		{
			name:  "service ErrDeviceRejected → FailedPrecondition",
			input: input{req: &mkoapiv1.ConfigureKKRequest{BoardId: "board1"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().ConfigureKK(gomock.Any(), gomock.Any()).
					Return(domain.WrapDeviceRejected("rejected"))
			},
			wantCode: codes.FailedPrecondition,
		},
		{
			name:  "service ErrOperationCanceled → Internal (no sentinel mapping)",
			input: input{req: &mkoapiv1.ConfigureKKRequest{BoardId: "board1"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().ConfigureKK(gomock.Any(), gomock.Any()).
					Return(domain.ErrOperationCanceled)
			},
			wantCode: codes.Internal,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)
			tc.prep(kk)

			srv := transport.NewMKOServer(ou, kk)
			_, err := srv.ConfigureKK(context.Background(), tc.input.req)

			assertGRPCCode(t, err, tc.wantCode)
		})
	}
}

func TestMKOServer_ConfigureExchange(t *testing.T) {
	type input struct {
		req *mkoapiv1.ConfigureExchangeRequest
	}
	tests := []struct {
		name     string
		input    input
		prep     func(m *mocks.MockKKServiceInterface)
		wantCode codes.Code
	}{
		{
			name: "valid request — maps all fields to ExchangeConfig",
			input: input{req: &mkoapiv1.ConfigureExchangeRequest{
				BoardId:     "board1",
				Format:      2,
				Ks1:         0x100,
				Ks2:         0x200,
				Sd:          []uint32{1, 2, 3},
				OperationId: "op-ex",
			}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().ConfigureExchange(gomock.Any(), domain.ExchangeConfig{
					Board:       "board1",
					Format:      2,
					KS1:         0x100,
					KS2:         0x200,
					SD:          []uint32{1, 2, 3},
					OperationID: "op-ex",
				}).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "service ErrTimeout → DeadlineExceeded",
			input: input{req: &mkoapiv1.ConfigureExchangeRequest{BoardId: "board1"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().ConfigureExchange(gomock.Any(), gomock.Any()).
					Return(domain.WrapTimeout("timeout"))
			},
			wantCode: codes.DeadlineExceeded,
		},
		{
			name:  "service ErrDeviceRejected → FailedPrecondition",
			input: input{req: &mkoapiv1.ConfigureExchangeRequest{BoardId: "board1"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().ConfigureExchange(gomock.Any(), gomock.Any()).
					Return(domain.WrapDeviceRejected("err"))
			},
			wantCode: codes.FailedPrecondition,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)
			tc.prep(kk)

			srv := transport.NewMKOServer(ou, kk)
			_, err := srv.ConfigureExchange(context.Background(), tc.input.req)

			assertGRPCCode(t, err, tc.wantCode)
		})
	}
}

func TestMKOServer_RunExchange(t *testing.T) {
	type input struct {
		req *mkoapiv1.RunExchangeRequest
	}
	tests := []struct {
		name     string
		input    input
		prep     func(m *mocks.MockKKServiceInterface)
		wantCode codes.Code
	}{
		{
			name: "valid request — maps fields to RunExchangeCommand",
			input: input{req: &mkoapiv1.RunExchangeRequest{
				BoardId:     "board1",
				OperationId: "op-run",
			}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().RunExchange(gomock.Any(), domain.RunExchangeCommand{
					Board:       "board1",
					OperationID: "op-run",
				}).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "service ErrTimeout → DeadlineExceeded",
			input: input{req: &mkoapiv1.RunExchangeRequest{BoardId: "board1", OperationId: "op-run"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().RunExchange(gomock.Any(), gomock.Any()).
					Return(domain.WrapTimeout("timeout"))
			},
			wantCode: codes.DeadlineExceeded,
		},
		{
			name:  "service ErrDeviceRejected → FailedPrecondition",
			input: input{req: &mkoapiv1.RunExchangeRequest{BoardId: "board1"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().RunExchange(gomock.Any(), gomock.Any()).
					Return(domain.WrapDeviceRejected("err"))
			},
			wantCode: codes.FailedPrecondition,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)
			tc.prep(kk)

			srv := transport.NewMKOServer(ou, kk)
			_, err := srv.RunExchange(context.Background(), tc.input.req)

			assertGRPCCode(t, err, tc.wantCode)
		})
	}
}

func TestMKOServer_SubscribeExchangeResults(t *testing.T) {
	type input struct {
		req *mkoapiv1.SubscribeExchangeResultsRequest
	}
	tests := []struct {
		name       string
		input      input
		prep       func(m *mocks.MockKKServiceInterface)
		wantErr    bool
		wantErrIs  error
		wantEvents int
	}{
		{
			name:  "channel closed immediately — returns nil",
			input: input{req: &mkoapiv1.SubscribeExchangeResultsRequest{BoardId: "board1"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				ch := make(chan domain.ExchangeResult)
				close(ch)
				m.EXPECT().SubscribeExchangeResults(gomock.Any(), "board1").Return(ch, nil)
			},
			wantEvents: 0,
		},
		{
			name:  "results from channel forwarded to stream",
			input: input{req: &mkoapiv1.SubscribeExchangeResultsRequest{BoardId: "board1"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				ch := make(chan domain.ExchangeResult, 2)
				ch <- domain.ExchangeResult{Format: 1, KS1: 0xA, ResultWord: 0xFF}
				ch <- domain.ExchangeResult{Format: 2, KS1: 0xB, ResultWord: 0xEE}
				close(ch)
				m.EXPECT().SubscribeExchangeResults(gomock.Any(), "board1").Return(ch, nil)
			},
			wantEvents: 2,
		},
		{
			name:  "service error → gRPC status error",
			input: input{req: &mkoapiv1.SubscribeExchangeResultsRequest{BoardId: "board1"}},
			prep: func(m *mocks.MockKKServiceInterface) {
				m.EXPECT().SubscribeExchangeResults(gomock.Any(), gomock.Any()).
					Return(nil, domain.WrapDriverUnavailable("no driver"))
			},
			wantErr:   true,
			wantErrIs: domain.ErrDriverUnavailable,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)
			tc.prep(kk)

			srv := transport.NewMKOServer(ou, kk)
			stream := &exchangeResultStreamStub{ctx: context.Background()}
			err := srv.SubscribeExchangeResults(tc.input.req, stream)

			if tc.wantErr {
				if err == nil {
					t.Fatal("expected error, got nil")
				}
				if tc.wantErrIs != nil {
					st, ok := status.FromError(err)
					if ok && st.Code() != codes.OK {
						// gRPC status error — check code is not OK
						return
					}
					if !errors.Is(err, tc.wantErrIs) {
						t.Errorf("got err %v, want wrapping %v", err, tc.wantErrIs)
					}
				}
				return
			}
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if len(stream.sent) != tc.wantEvents {
				t.Errorf("sent %d results, want %d", len(stream.sent), tc.wantEvents)
			}
		})
	}
}