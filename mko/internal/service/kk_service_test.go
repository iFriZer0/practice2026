package service_test

import (
	"context"
	"errors"
	"testing"

	"mko/internal/domain"
	"mko/internal/service"
	"mko/internal/service/mocks"

	"go.uber.org/mock/gomock"
)

func TestKKService_ConfigureKK(t *testing.T) {
	type input struct {
		cfg domain.KKConfig
	}
	tests := []struct {
		name    string
		input   input
		prep    func(m *mocks.MockKKDriver)
		wantErr error
	}{
		{
			name:  "success — driver ConfigureMko called",
			input: input{cfg: domain.KKConfig{Board: "board1", OperationID: "op-1"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().ConfigureMko(gomock.Any(), gomock.Any()).Return(nil)
			},
		},
		{
			name:  "driver returns DeadlineExceeded → ErrTimeout",
			input: input{cfg: domain.KKConfig{Board: "board1", OperationID: "op-1"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().ConfigureMko(gomock.Any(), gomock.Any()).Return(context.DeadlineExceeded)
			},
			wantErr: domain.ErrTimeout,
		},
		{
			name:  "driver returns Canceled → ErrOperationCanceled",
			input: input{cfg: domain.KKConfig{Board: "board1", OperationID: "op-1"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().ConfigureMko(gomock.Any(), gomock.Any()).Return(context.Canceled)
			},
			wantErr: domain.ErrOperationCanceled,
		},
		{
			name:  "driver returns arbitrary error → ErrDeviceRejected",
			input: input{cfg: domain.KKConfig{Board: "board1", OperationID: "op-1"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().ConfigureMko(gomock.Any(), gomock.Any()).Return(errors.New("hardware fault"))
			},
			wantErr: domain.ErrDeviceRejected,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockKKDriver(ctrl)
			tc.prep(m)

			err := service.NewKKService(m).ConfigureKK(context.Background(), tc.input.cfg)

			checkKKErr(t, err, tc.wantErr)
		})
	}
}

func TestKKService_ConfigureExchange(t *testing.T) {
	type input struct {
		cfg domain.ExchangeConfig
	}
	tests := []struct {
		name    string
		input   input
		prep    func(m *mocks.MockKKDriver)
		wantErr error
	}{
		{
			name:  "success — driver ConfigureExchange called",
			input: input{cfg: domain.ExchangeConfig{Board: "board1", OperationID: "op-2", Format: 1, KS1: 0x100}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().ConfigureExchange(gomock.Any(), gomock.Any()).Return(nil)
			},
		},
		{
			name:  "driver returns DeadlineExceeded → ErrTimeout",
			input: input{cfg: domain.ExchangeConfig{Board: "board1", OperationID: "op-2"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().ConfigureExchange(gomock.Any(), gomock.Any()).Return(context.DeadlineExceeded)
			},
			wantErr: domain.ErrTimeout,
		},
		{
			name:  "driver returns Canceled → ErrOperationCanceled",
			input: input{cfg: domain.ExchangeConfig{Board: "board1", OperationID: "op-2"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().ConfigureExchange(gomock.Any(), gomock.Any()).Return(context.Canceled)
			},
			wantErr: domain.ErrOperationCanceled,
		},
		{
			name:  "driver returns arbitrary error → ErrDeviceRejected",
			input: input{cfg: domain.ExchangeConfig{Board: "board1"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().ConfigureExchange(gomock.Any(), gomock.Any()).Return(errors.New("device error"))
			},
			wantErr: domain.ErrDeviceRejected,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockKKDriver(ctrl)
			tc.prep(m)

			err := service.NewKKService(m).ConfigureExchange(context.Background(), tc.input.cfg)

			checkKKErr(t, err, tc.wantErr)
		})
	}
}

func TestKKService_RunExchange(t *testing.T) {
	type input struct {
		cmd domain.RunExchangeCommand
	}
	tests := []struct {
		name    string
		input   input
		prep    func(m *mocks.MockKKDriver)
		wantErr error
	}{
		{
			name:  "success — driver SendCommandRun called",
			input: input{cmd: domain.RunExchangeCommand{Board: "board1", OperationID: "op-3"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().SendCommandRun(gomock.Any(), gomock.Any()).Return(nil)
			},
		},
		{
			name:  "driver returns DeadlineExceeded → ErrTimeout",
			input: input{cmd: domain.RunExchangeCommand{Board: "board1", OperationID: "op-3"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().SendCommandRun(gomock.Any(), gomock.Any()).Return(context.DeadlineExceeded)
			},
			wantErr: domain.ErrTimeout,
		},
		{
			name:  "driver returns Canceled → ErrOperationCanceled",
			input: input{cmd: domain.RunExchangeCommand{Board: "board1", OperationID: "op-3"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().SendCommandRun(gomock.Any(), gomock.Any()).Return(context.Canceled)
			},
			wantErr: domain.ErrOperationCanceled,
		},
		{
			name:  "driver returns arbitrary error → ErrDeviceRejected",
			input: input{cmd: domain.RunExchangeCommand{Board: "board1"}},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().SendCommandRun(gomock.Any(), gomock.Any()).Return(errors.New("exchange failed"))
			},
			wantErr: domain.ErrDeviceRejected,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockKKDriver(ctrl)
			tc.prep(m)

			err := service.NewKKService(m).RunExchange(context.Background(), tc.input.cmd)

			checkKKErr(t, err, tc.wantErr)
		})
	}
}

func TestKKService_SubscribeExchangeResults(t *testing.T) {
	type input struct {
		boardID string
	}
	tests := []struct {
		name    string
		input   input
		prep    func(m *mocks.MockKKDriver)
		wantCh  bool
		wantErr error
	}{
		{
			name:  "returns channel from driver as-is",
			input: input{boardID: "board1"},
			prep: func(m *mocks.MockKKDriver) {
				ch := make(chan domain.ExchangeResult)
				m.EXPECT().SubscribeExchangeResults(gomock.Any(), "board1").Return(ch, nil)
			},
			wantCh: true,
		},
		{
			name:  "driver error → ErrDeviceRejected",
			input: input{boardID: "board1"},
			prep: func(m *mocks.MockKKDriver) {
				m.EXPECT().SubscribeExchangeResults(gomock.Any(), gomock.Any()).
					Return(nil, errors.New("subscribe failed"))
			},
			wantErr: domain.ErrDeviceRejected,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockKKDriver(ctrl)
			tc.prep(m)

			ch, err := service.NewKKService(m).SubscribeExchangeResults(context.Background(), tc.input.boardID)

			if tc.wantErr != nil {
				if !errors.Is(err, tc.wantErr) {
					t.Errorf("got err %v, want wrapping %v", err, tc.wantErr)
				}
				return
			}
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if tc.wantCh && ch == nil {
				t.Error("expected non-nil channel, got nil")
			}
		})
	}
}

func checkKKErr(t *testing.T, got, want error) {
	t.Helper()
	if want != nil {
		if !errors.Is(got, want) {
			t.Errorf("got err %v, want error wrapping %v", got, want)
		}
		return
	}
	if got != nil {
		t.Errorf("unexpected error: %v", got)
	}
}
