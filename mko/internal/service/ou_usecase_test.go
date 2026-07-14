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

func TestOuService_GetVersion(t *testing.T) {
	tests := []struct {
		name    string
		prep    func(m *mocks.MockDriverOuClient)
		want    string
		wantErr error
	}{
		{
			name: "driver returns version string",
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().GetVersion(gomock.Any()).Return("1.2.3", nil)
			},
			want: "1.2.3",
		},
		{
			name: "driver unavailable propagates error",
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().GetVersion(gomock.Any()).Return("", domain.ErrDriverUnavailable)
			},
			wantErr: domain.ErrDriverUnavailable,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockDriverOuClient(ctrl)
			tc.prep(m)

			got, err := service.NewOuService(m).GetVersion(context.Background())

			if tc.wantErr != nil {
				if !errors.Is(err, tc.wantErr) {
					t.Errorf("got err %v, want wrapping %v", err, tc.wantErr)
				}
				return
			}
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if got != tc.want {
				t.Errorf("got %q, want %q", got, tc.want)
			}
		})
	}
}

func TestOuService_Ping(t *testing.T) {
	tests := []struct {
		name    string
		prep    func(m *mocks.MockDriverOuClient)
		wantErr error
	}{
		{
			name: "driver responds ok",
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().Ping(gomock.Any()).Return(nil)
			},
		},
		{
			name: "driver unavailable propagates error",
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().Ping(gomock.Any()).Return(domain.ErrDriverUnavailable)
			},
			wantErr: domain.ErrDriverUnavailable,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockDriverOuClient(ctrl)
			tc.prep(m)

			err := service.NewOuService(m).Ping(context.Background())

			checkErr(t, err, tc.wantErr)
		})
	}
}

func TestOuService_ConfigureOu(t *testing.T) {
	type input struct {
		cfg domain.OuConfig
	}
	tests := []struct {
		name    string
		input   input
		prep    func(m *mocks.MockDriverOuClient)
		wantErr error
	}{
		{
			name:  "valid config — delegates to driver as-is",
			input: input{cfg: domain.OuConfig{Board: "brd", Index: 2, Channel: 0, OuAddress: 5}},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().ConfigureOu(gomock.Any(),
					domain.OuConfig{Board: "brd", Index: 2, Channel: 0, OuAddress: 5},
				).Return(nil)
			},
		},
		{
			name:  "index = 0 normalizes to 1 before driver call",
			input: input{cfg: domain.OuConfig{Board: "brd", Index: 0}},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().ConfigureOu(gomock.Any(),
					domain.OuConfig{Board: "brd", Index: 1},
				).Return(nil)
			},
		},
		{
			name:  "negative index normalizes to 1 before driver call",
			input: input{cfg: domain.OuConfig{Board: "brd", Index: -99}},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().ConfigureOu(gomock.Any(),
					domain.OuConfig{Board: "brd", Index: 1},
				).Return(nil)
			},
		},
		{
			name:  "driver error is returned unchanged",
			input: input{cfg: domain.OuConfig{Board: "brd", Index: 1}},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().ConfigureOu(gomock.Any(), gomock.Any()).Return(domain.ErrDeviceRejected)
			},
			wantErr: domain.ErrDeviceRejected,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockDriverOuClient(ctrl)
			tc.prep(m)

			err := service.NewOuService(m).ConfigureOu(context.Background(), tc.input.cfg)

			checkErr(t, err, tc.wantErr)
		})
	}
}

func TestOuService_SetOuResponseWord(t *testing.T) {
	type input struct {
		board     domain.BoardID
		index     domain.McoIndex
		channel   domain.Channel
		ouAddress int32
		word      int32
	}
	tests := []struct {
		name    string
		input   input
		prep    func(m *mocks.MockDriverOuClient)
		wantErr error
	}{
		{
			name:  "valid — delegates to driver with exact args",
			input: input{board: "brd", index: 2, channel: 1, ouAddress: 3, word: 100},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().SetOuResponseWord(
					gomock.Any(),
					domain.BoardID("brd"), domain.McoIndex(2), domain.Channel(1), int32(3), int32(100),
				).Return(nil)
			},
		},
		{
			name:  "index = 0 normalizes to 1 before driver call",
			input: input{board: "brd", index: 0, channel: 0, ouAddress: 5, word: 0},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().SetOuResponseWord(
					gomock.Any(),
					domain.BoardID("brd"), domain.McoIndex(1), domain.Channel(0), int32(5), int32(0),
				).Return(nil)
			},
		},
		{
			name:  "negative index normalizes to 1 before driver call",
			input: input{board: "brd", index: -7, channel: 1, ouAddress: 10, word: 42},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().SetOuResponseWord(
					gomock.Any(),
					domain.BoardID("brd"), domain.McoIndex(1), domain.Channel(1), int32(10), int32(42),
				).Return(nil)
			},
		},
		{
			name:  "driver error is returned unchanged",
			input: input{board: "brd", index: 1},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().SetOuResponseWord(
					gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any(),
				).Return(domain.ErrDeviceRejected)
			},
			wantErr: domain.ErrDeviceRejected,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockDriverOuClient(ctrl)
			tc.prep(m)

			err := service.NewOuService(m).SetOuResponseWord(
				context.Background(),
				tc.input.board, tc.input.index, tc.input.channel,
				tc.input.ouAddress, tc.input.word,
			)

			checkErr(t, err, tc.wantErr)
		})
	}
}

func TestOuService_ReadOuSubaddress(t *testing.T) {
	type input struct {
		board       domain.BoardID
		sub         uint32
		receiveArea bool
	}
	wantData := domain.OuSubaddressData{Subaddress: 5, Sd: []uint32{1, 2, 3}, CmdWord: 0xAB}

	tests := []struct {
		name    string
		input   input
		prep    func(m *mocks.MockDriverOuClient)
		want    domain.OuSubaddressData
		wantErr error
	}{
		{
			name:  "delegates to driver and returns data unchanged",
			input: input{board: "brd", sub: 5, receiveArea: true},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().ReadOuSubaddress(
					gomock.Any(), domain.BoardID("brd"), uint32(5), true,
				).Return(wantData, nil)
			},
			want: wantData,
		},
		{
			name:  "receive_area=false passed through",
			input: input{board: "brd", sub: 10, receiveArea: false},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().ReadOuSubaddress(
					gomock.Any(), domain.BoardID("brd"), uint32(10), false,
				).Return(domain.OuSubaddressData{Subaddress: 10}, nil)
			},
			want: domain.OuSubaddressData{Subaddress: 10},
		},
		{
			name:  "driver error is returned unchanged",
			input: input{board: "brd", sub: 5, receiveArea: true},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().ReadOuSubaddress(gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any()).
					Return(domain.OuSubaddressData{}, domain.ErrDriverUnavailable)
			},
			wantErr: domain.ErrDriverUnavailable,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockDriverOuClient(ctrl)
			tc.prep(m)

			got, err := service.NewOuService(m).ReadOuSubaddress(
				context.Background(), tc.input.board, tc.input.sub, tc.input.receiveArea,
			)

			if tc.wantErr != nil {
				if !errors.Is(err, tc.wantErr) {
					t.Errorf("got err %v, want wrapping %v", err, tc.wantErr)
				}
				return
			}
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if got.Subaddress != tc.want.Subaddress || got.CmdWord != tc.want.CmdWord {
				t.Errorf("got %+v, want %+v", got, tc.want)
			}
		})
	}
}

func TestOuService_WriteOuSubaddress(t *testing.T) {
	type input struct {
		board domain.BoardID
		sub   uint32
		sd    []uint32
	}
	tests := []struct {
		name    string
		input   input
		prep    func(m *mocks.MockDriverOuClient)
		wantErr error
	}{
		{
			name:  "delegates to driver with exact args",
			input: input{board: "brd", sub: 3, sd: []uint32{10, 20, 30}},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().WriteOuSubaddress(
					gomock.Any(), domain.BoardID("brd"), uint32(3), []uint32{10, 20, 30},
				).Return(nil)
			},
		},
		{
			name:  "driver error is returned unchanged",
			input: input{board: "brd", sub: 3, sd: []uint32{1}},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().WriteOuSubaddress(
					gomock.Any(), gomock.Any(), gomock.Any(), gomock.Any(),
				).Return(domain.ErrDeviceRejected)
			},
			wantErr: domain.ErrDeviceRejected,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockDriverOuClient(ctrl)
			tc.prep(m)

			err := service.NewOuService(m).WriteOuSubaddress(
				context.Background(), tc.input.board, tc.input.sub, tc.input.sd,
			)

			checkErr(t, err, tc.wantErr)
		})
	}
}

func TestOuService_SubscribeOuCommands(t *testing.T) {
	type input struct {
		board domain.BoardID
	}
	tests := []struct {
		name    string
		input   input
		prep    func(m *mocks.MockDriverOuClient)
		wantCh  bool
		wantErr error
	}{
		{
			name:  "returns channel from driver as-is",
			input: input{board: "brd"},
			prep: func(m *mocks.MockDriverOuClient) {
				ch := make(chan domain.OuCommandEvent)
				m.EXPECT().SubscribeOuCommands(gomock.Any(), domain.BoardID("brd")).Return(ch, nil)
			},
			wantCh: true,
		},
		{
			name:  "driver error is returned unchanged",
			input: input{board: "brd"},
			prep: func(m *mocks.MockDriverOuClient) {
				m.EXPECT().SubscribeOuCommands(gomock.Any(), gomock.Any()).
					Return(nil, domain.ErrDriverUnavailable)
			},
			wantErr: domain.ErrDriverUnavailable,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			ctrl := gomock.NewController(t)
			m := mocks.NewMockDriverOuClient(ctrl)
			tc.prep(m)

			ch, err := service.NewOuService(m).SubscribeOuCommands(context.Background(), tc.input.board)

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

func checkErr(t *testing.T, got, want error) {
	t.Helper()
	if want != nil {
		if !errors.Is(got, want) {
			t.Errorf("got err %v, want wrapping %v", got, want)
		}
		return
	}
	if got != nil {
		t.Errorf("unexpected error: %v", got)
	}
}