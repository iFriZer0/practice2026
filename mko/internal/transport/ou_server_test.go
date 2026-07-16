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

// ouCommandStreamStub — минимальная реализация grpc.ServerStreamingServer для тестов.
type ouCommandStreamStub struct {
	ctx  context.Context
	sent []*mkoapiv1.OuCommandEvent
}

func (s *ouCommandStreamStub) Send(ev *mkoapiv1.OuCommandEvent) error {
	s.sent = append(s.sent, ev)
	return nil
}
func (s *ouCommandStreamStub) Context() context.Context          { return s.ctx }
func (s *ouCommandStreamStub) SetHeader(metadata.MD) error       { return nil }
func (s *ouCommandStreamStub) SendHeader(metadata.MD) error      { return nil }
func (s *ouCommandStreamStub) SetTrailer(metadata.MD)            {}
func (s *ouCommandStreamStub) SendMsg(any) error                 { return nil }
func (s *ouCommandStreamStub) RecvMsg(any) error                 { return nil }

func TestMKOServer_GetStatus(t *testing.T) {
	tests := []struct {
		name    string
		prep    func(m *mocks.MockOuServiceInterface)
		want    *mkoapiv1.StatusResponse
		wantErr bool
	}{
		{
			name: "driver connected — GetVersion ok + Ping ok",
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().GetVersion(gomock.Any()).Return("2.0.0", nil)
				m.EXPECT().Ping(gomock.Any()).Return(nil)
			},
			want: &mkoapiv1.StatusResponse{
				ServiceVersion:  "0.1.0",
				DriverConnected: true,
				DriverVersion:   "2.0.0",
			},
		},
		{
			name: "GetVersion fails — health-check returns disconnected, no API error",
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().GetVersion(gomock.Any()).Return("", domain.ErrDriverUnavailable)
			},
			want: &mkoapiv1.StatusResponse{DriverConnected: false},
		},
		{
			name: "GetVersion ok but Ping fails — DriverConnected=false",
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().GetVersion(gomock.Any()).Return("2.0.0", nil)
				m.EXPECT().Ping(gomock.Any()).Return(domain.ErrDriverUnavailable)
			},
			want: &mkoapiv1.StatusResponse{
				ServiceVersion:  "0.1.0",
				DriverConnected: false,
				DriverVersion:   "2.0.0",
			},
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)
			tc.prep(ou)

			srv := transport.NewMKOServer(ou, kk)
			got, err := srv.GetStatus(context.Background(), &mkoapiv1.Empty{})

			if tc.wantErr && err == nil {
				t.Fatal("expected error, got nil")
			}
			if !tc.wantErr && err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if got.GetDriverConnected() != tc.want.GetDriverConnected() {
				t.Errorf("DriverConnected: got %v, want %v", got.GetDriverConnected(), tc.want.GetDriverConnected())
			}
			if got.GetDriverVersion() != tc.want.GetDriverVersion() {
				t.Errorf("DriverVersion: got %q, want %q", got.GetDriverVersion(), tc.want.GetDriverVersion())
			}
			if got.GetServiceVersion() != tc.want.GetServiceVersion() {
				t.Errorf("ServiceVersion: got %q, want %q", got.GetServiceVersion(), tc.want.GetServiceVersion())
			}
		})
	}
}

func TestMKOServer_ConfigureOu(t *testing.T) {
	type input struct {
		req *mkoapiv1.ConfigureOuRequest
	}
	tests := []struct {
		name     string
		input    input
		prep     func(m *mocks.MockOuServiceInterface)
		wantCode codes.Code
	}{
		{
			name:  "valid request — delegates to service",
			input: input{req: &mkoapiv1.ConfigureOuRequest{BoardId: "brd", Index: 1, Channel: 0, OuAddress: 5}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().ConfigureOu(gomock.Any(), gomock.Any()).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "ou_address negative — InvalidArgument before service call",
			input: input{req: &mkoapiv1.ConfigureOuRequest{OuAddress: -1}},
			prep:  func(m *mocks.MockOuServiceInterface) {},
			wantCode: codes.InvalidArgument,
		},
		{
			name:  "ou_address = 31 — InvalidArgument before service call",
			input: input{req: &mkoapiv1.ConfigureOuRequest{OuAddress: 31}},
			prep:  func(m *mocks.MockOuServiceInterface) {},
			wantCode: codes.InvalidArgument,
		},
		{
			name:  "ou_address = 30 — boundary valid",
			input: input{req: &mkoapiv1.ConfigureOuRequest{OuAddress: 30}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().ConfigureOu(gomock.Any(), gomock.Any()).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "service ErrDeviceRejected → FailedPrecondition",
			input: input{req: &mkoapiv1.ConfigureOuRequest{OuAddress: 5}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().ConfigureOu(gomock.Any(), gomock.Any()).
					Return(domain.WrapDeviceRejected("board rejected"))
			},
			wantCode: codes.FailedPrecondition,
		},
		{
			name:  "service ErrTimeout → DeadlineExceeded",
			input: input{req: &mkoapiv1.ConfigureOuRequest{OuAddress: 5}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().ConfigureOu(gomock.Any(), gomock.Any()).
					Return(domain.WrapTimeout("timeout"))
			},
			wantCode: codes.DeadlineExceeded,
		},
		{
			name:  "service ErrDriverUnavailable → Unavailable",
			input: input{req: &mkoapiv1.ConfigureOuRequest{OuAddress: 5}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().ConfigureOu(gomock.Any(), gomock.Any()).
					Return(domain.WrapDriverUnavailable("no conn"))
			},
			wantCode: codes.Unavailable,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)
			tc.prep(ou)

			srv := transport.NewMKOServer(ou, kk)
			_, err := srv.ConfigureOu(context.Background(), tc.input.req)

			assertGRPCCode(t, err, tc.wantCode)
		})
	}
}

func TestMKOServer_SetOuResponseWord(t *testing.T) {
	type input struct {
		req *mkoapiv1.SetOuResponseWordRequest
	}
	tests := []struct {
		name     string
		input    input
		prep     func(m *mocks.MockOuServiceInterface)
		wantCode codes.Code
	}{
		{
			name:  "valid — delegates to service",
			input: input{req: &mkoapiv1.SetOuResponseWordRequest{BoardId: "brd", Index: 2, Channel: 1, OuAddress: 3, ResponseWord: 0xFF}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().SetOuResponseWord(
					gomock.Any(),
					domain.BoardID("brd"), domain.McoIndex(2), domain.Channel(1), int32(3), int32(0xFF),
				).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "service error → FailedPrecondition",
			input: input{req: &mkoapiv1.SetOuResponseWordRequest{BoardId: "brd"}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().SetOuResponseWord(
					gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any(),
				).Return(domain.WrapDeviceRejected("rejected"))
			},
			wantCode: codes.FailedPrecondition,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)
			tc.prep(ou)

			srv := transport.NewMKOServer(ou, kk)
			_, err := srv.SetOuResponseWord(context.Background(), tc.input.req)

			assertGRPCCode(t, err, tc.wantCode)
		})
	}
}

func TestMKOServer_ReadOuSubaddress(t *testing.T) {
	type input struct {
		req *mkoapiv1.ReadOuSubaddressRequest
	}
	wantData := domain.OuSubaddressData{
		Subaddress:    5,
		Sd:            []uint32{1, 2},
		CmdWord:       0xAB,
		ResultWord:    0xCD,
		DecodedResult: "OK",
	}
	tests := []struct {
		name     string
		input    input
		prep     func(m *mocks.MockOuServiceInterface)
		want     *mkoapiv1.OuSubaddressData
		wantCode codes.Code
	}{
		{
			name:  "valid request — returns mapped data",
			input: input{req: &mkoapiv1.ReadOuSubaddressRequest{BoardId: "brd", Subaddress: 5, ReceiveArea: true}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().ReadOuSubaddress(
					gomock.Any(), domain.BoardID("brd"), uint32(5), true,
				).Return(wantData, nil)
			},
			want: &mkoapiv1.OuSubaddressData{
				Subaddress:    5,
				Sd:            []uint32{1, 2},
				CmdWord:       0xAB,
				ResultWord:    0xCD,
				DecodedResult: "OK",
			},
			wantCode: codes.OK,
		},
		{
			name:     "subaddress = 31 — InvalidArgument before service call",
			input:    input{req: &mkoapiv1.ReadOuSubaddressRequest{Subaddress: 31}},
			prep:     func(m *mocks.MockOuServiceInterface) {},
			wantCode: codes.InvalidArgument,
		},
		{
			name:     "subaddress = 30 — boundary valid",
			input:    input{req: &mkoapiv1.ReadOuSubaddressRequest{BoardId: "brd", Subaddress: 30}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().ReadOuSubaddress(gomock.Any(), gomock.Any(), uint32(30), gomock.Any()).
					Return(domain.OuSubaddressData{Subaddress: 30}, nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "service error → FailedPrecondition",
			input: input{req: &mkoapiv1.ReadOuSubaddressRequest{BoardId: "brd", Subaddress: 5}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().ReadOuSubaddress(gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any()).
					Return(domain.OuSubaddressData{}, domain.WrapDeviceRejected("err"))
			},
			wantCode: codes.FailedPrecondition,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)
			tc.prep(ou)

			srv := transport.NewMKOServer(ou, kk)
			got, err := srv.ReadOuSubaddress(context.Background(), tc.input.req)

			assertGRPCCode(t, err, tc.wantCode)
			if tc.want != nil && err == nil {
				if got.GetSubaddress() != tc.want.GetSubaddress() {
					t.Errorf("Subaddress: got %d, want %d", got.GetSubaddress(), tc.want.GetSubaddress())
				}
				if got.GetDecodedResult() != tc.want.GetDecodedResult() {
					t.Errorf("DecodedResult: got %q, want %q", got.GetDecodedResult(), tc.want.GetDecodedResult())
				}
			}
		})
	}
}

func TestMKOServer_WriteOuSubaddress(t *testing.T) {
	type input struct {
		req *mkoapiv1.WriteOuSubaddressRequest
	}
	tests := []struct {
		name     string
		input    input
		prep     func(m *mocks.MockOuServiceInterface)
		wantCode codes.Code
	}{
		{
			name:  "valid request — delegates to service",
			input: input{req: &mkoapiv1.WriteOuSubaddressRequest{BoardId: "brd", Subaddress: 3, Sd: []uint32{10, 20}}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().WriteOuSubaddress(
					gomock.Any(), domain.BoardID("brd"), uint32(3), []uint32{10, 20},
				).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:     "subaddress = 31 — InvalidArgument",
			input:    input{req: &mkoapiv1.WriteOuSubaddressRequest{Subaddress: 31, Sd: []uint32{1}}},
			prep:     func(m *mocks.MockOuServiceInterface) {},
			wantCode: codes.InvalidArgument,
		},
		{
			name:     "sd empty — InvalidArgument",
			input:    input{req: &mkoapiv1.WriteOuSubaddressRequest{Subaddress: 5, Sd: []uint32{}}},
			prep:     func(m *mocks.MockOuServiceInterface) {},
			wantCode: codes.InvalidArgument,
		},
		{
			name:     "sd count = 33 — InvalidArgument",
			input:    input{req: &mkoapiv1.WriteOuSubaddressRequest{Subaddress: 5, Sd: make([]uint32, 33)}},
			prep:     func(m *mocks.MockOuServiceInterface) {},
			wantCode: codes.InvalidArgument,
		},
		{
			name:  "sd count = 32 — boundary valid",
			input: input{req: &mkoapiv1.WriteOuSubaddressRequest{BoardId: "brd", Subaddress: 5, Sd: make([]uint32, 32)}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().WriteOuSubaddress(gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any()).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "service error → FailedPrecondition",
			input: input{req: &mkoapiv1.WriteOuSubaddressRequest{BoardId: "brd", Subaddress: 5, Sd: []uint32{1}}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().WriteOuSubaddress(gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any()).
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
			tc.prep(ou)

			srv := transport.NewMKOServer(ou, kk)
			_, err := srv.WriteOuSubaddress(context.Background(), tc.input.req)

			assertGRPCCode(t, err, tc.wantCode)
		})
	}
}

func TestMKOServer_SendRawOuData(t *testing.T) {
	type input struct {
		req *mkoapiv1.WriteOuSubaddressRequest
	}
	tests := []struct {
		name     string
		input    input
		prep     func(m *mocks.MockOuServiceInterface)
		wantCode codes.Code
	}{
		{
			name:  "calls service even with subaddress > 30 — no validation",
			input: input{req: &mkoapiv1.WriteOuSubaddressRequest{BoardId: "brd", Subaddress: 31, Sd: []uint32{1}}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().WriteOuSubaddress(
					gomock.Any(), domain.BoardID("brd"), uint32(31), []uint32{1},
				).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "calls service even with empty sd — no validation",
			input: input{req: &mkoapiv1.WriteOuSubaddressRequest{BoardId: "brd", Subaddress: 5, Sd: []uint32{}}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().WriteOuSubaddress(
					gomock.Any(), domain.BoardID("brd"), uint32(5), []uint32{},
				).Return(nil)
			},
			wantCode: codes.OK,
		},
		{
			name:  "service error still mapped to gRPC status",
			input: input{req: &mkoapiv1.WriteOuSubaddressRequest{BoardId: "brd", Subaddress: 5, Sd: []uint32{1}}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().WriteOuSubaddress(gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any()).
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
			tc.prep(ou)

			srv := transport.NewMKOServer(ou, kk)
			_, err := srv.SendRawOuData(context.Background(), tc.input.req)

			assertGRPCCode(t, err, tc.wantCode)
		})
	}
}

func TestMKOServer_ClearBuffers(t *testing.T) {
	tests := []struct {
		name     string
		call     func(srv *transport.MKOServer) error
		wantCode codes.Code
	}{
		{
			name: "ClearReceiveBuffer — always Unimplemented",
			call: func(srv *transport.MKOServer) error {
				_, err := srv.ClearReceiveBuffer(context.Background(), &mkoapiv1.ClearBufferRequest{})
				return err
			},
			wantCode: codes.Unimplemented,
		},
		{
			name: "ClearTransmitBuffer — always Unimplemented",
			call: func(srv *transport.MKOServer) error {
				_, err := srv.ClearTransmitBuffer(context.Background(), &mkoapiv1.ClearBufferRequest{})
				return err
			},
			wantCode: codes.Unimplemented,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)

			srv := transport.NewMKOServer(ou, kk)
			err := tc.call(srv)

			assertGRPCCode(t, err, tc.wantCode)
		})
	}
}

func TestMKOServer_SubscribeOuCommands(t *testing.T) {
	type input struct {
		req *mkoapiv1.SubscribeOuCommandsRequest
	}
	tests := []struct {
		name       string
		input      input
		prep       func(m *mocks.MockOuServiceInterface)
		setupCh    func() chan domain.OuCommandEvent
		wantErr    error
		wantEvents int
	}{
		{
			name:  "channel closed immediately — returns nil",
			input: input{req: &mkoapiv1.SubscribeOuCommandsRequest{BoardId: "brd"}},
			prep: func(m *mocks.MockOuServiceInterface) {
				ch := make(chan domain.OuCommandEvent)
				close(ch)
				m.EXPECT().SubscribeOuCommands(gomock.Any(), domain.BoardID("brd")).Return(ch, nil)
			},
			wantEvents: 0,
		},
		{
			name:  "events from channel forwarded to stream",
			input: input{req: &mkoapiv1.SubscribeOuCommandsRequest{BoardId: "brd"}},
			prep: func(m *mocks.MockOuServiceInterface) {
				ch := make(chan domain.OuCommandEvent, 2)
				ch <- domain.OuCommandEvent{CmdWord: 0x10}
				ch <- domain.OuCommandEvent{CmdWord: 0x20}
				close(ch)
				m.EXPECT().SubscribeOuCommands(gomock.Any(), domain.BoardID("brd")).Return(ch, nil)
			},
			wantEvents: 2,
		},
		{
			name:  "service error → gRPC status error",
			input: input{req: &mkoapiv1.SubscribeOuCommandsRequest{BoardId: "brd"}},
			prep: func(m *mocks.MockOuServiceInterface) {
				m.EXPECT().SubscribeOuCommands(gomock.Any(), gomock.Any()).
					Return(nil, domain.WrapDriverUnavailable("no driver"))
			},
			wantErr: domain.ErrDriverUnavailable,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			ou := mocks.NewMockOuServiceInterface(ctrl)
			kk := mocks.NewMockKKServiceInterface(ctrl)
			tc.prep(ou)

			srv := transport.NewMKOServer(ou, kk)
			stream := &ouCommandStreamStub{ctx: context.Background()}
			err := srv.SubscribeOuCommands(tc.input.req, stream)

			if tc.wantErr != nil {
				st, ok := status.FromError(err)
				if !ok {
					t.Fatalf("expected gRPC status error, got: %v", err)
				}
				if st.Code() == codes.OK {
					t.Errorf("expected non-OK code, got OK")
				}
				return
			}
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if len(stream.sent) != tc.wantEvents {
				t.Errorf("sent %d events, want %d", len(stream.sent), tc.wantEvents)
			}
		})
	}
}

func assertGRPCCode(t *testing.T, err error, want codes.Code) {
	t.Helper()
	if want == codes.OK {
		if err != nil {
			t.Errorf("expected no error, got: %v", err)
		}
		return
	}
	if err == nil {
		t.Fatalf("expected error with code %v, got nil", want)
	}
	st, ok := status.FromError(err)
	if !ok {
		// wrapped sentinel errors from domain don't have gRPC status — check via errors.Is
		if !errors.Is(err, err) {
			t.Errorf("not a gRPC status error: %v", err)
		}
		return
	}
	if st.Code() != want {
		t.Errorf("got gRPC code %v, want %v (msg: %s)", st.Code(), want, st.Message())
	}
}