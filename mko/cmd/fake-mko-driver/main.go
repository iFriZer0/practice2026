package main

import (
	"context"
	"log"
	"mko/internal/transport/clients/mko-driver-service/gen/mkodriverv1"
	"net"
	"time"

	"google.golang.org/grpc"
	"google.golang.org/protobuf/types/known/emptypb"
)

type fakeDriverServer struct {
	mkodriverv1.UnimplementedMkoDriverServer
}

func (s *fakeDriverServer) ConfigureMko(
	ctx context.Context, req *mkodriverv1.MkoConfigRequest) (*mkodriverv1.StandardResponse, error) {
	log.Printf("ConfigureMko: %+v", req)

	return &mkodriverv1.StandardResponse{
		Success: true,
	}, nil
}

func (s *fakeDriverServer) ConfigureExchange(ctx context.Context, req *mkodriverv1.ExchangeConfigRequest) (*mkodriverv1.StandardResponse, error) {
	log.Printf("ConfigureExchange: %+v", req)
	return &mkodriverv1.StandardResponse{
		Success: true,
	}, nil
}

func (s *fakeDriverServer) SendCommandRun(ctx context.Context, req *mkodriverv1.ActionRequest) (*mkodriverv1.StandardResponse, error) {
	log.Printf("SendCommandRun: %+v", req)
	return &mkodriverv1.StandardResponse{
		Success: true,
	}, nil
}

func (s *fakeDriverServer) ReadExchangeConfig(ctx context.Context, req *mkodriverv1.ActionRequest) (*mkodriverv1.StandardResponse, error) {
	log.Printf("ReadExchangeConfig: %+v", req)
	return &mkodriverv1.StandardResponse{
		Success: true,
	}, nil
}

func (s *fakeDriverServer) SubscribeExchangeResults(
	req *mkodriverv1.Empty,
	stream mkodriverv1.MkoDriver_SubscribeExchangeResultsServer,
) error {
	log.Printf("SubscribeExchangeResults")
	return s.sendExchangeResults(stream.Context(), func(result *mkodriverv1.ExchangeResultResponse) error {
		return stream.Send(result)
	})
}

func (s *fakeDriverServer) SubscribeExchangeResultsForBoard(
	req *mkodriverv1.BoardRequest,
	stream mkodriverv1.MkoDriver_SubscribeExchangeResultsForBoardServer,
) error {
	log.Printf("SubscribeExchangeResultsForBoard: board=%s", req.GetBoardId())
	return s.sendExchangeResults(stream.Context(), func(result *mkodriverv1.ExchangeResultResponse) error {
		return stream.Send(result)
	})
}

func (s *fakeDriverServer) sendExchangeResults(ctx context.Context, send func(*mkodriverv1.ExchangeResultResponse) error) error {
	ticker := time.NewTicker(time.Second)
	defer ticker.Stop()

	for i := 0; ; i++ {
		select {
		case <-ctx.Done():
			return ctx.Err()

		case <-ticker.C:
			err := send(&mkodriverv1.ExchangeResultResponse{
				Format:        1,
				Ks1:           0x1000,
				Ks2:           0x2000,
				Sd:            []uint32{uint32(i), 2, 3, 4},
				AnswerWord_1:  0xAAAA,
				AnswerWord_2:  0xBBBB,
				ResultWord:    0,
				DecodedResult: "fake exchange result ok",
			})
			if err != nil {
				return err
			}
		}
	}
}

func (s *fakeDriverServer) ConfigureOu(ctx context.Context, req *mkodriverv1.ConfigureOuRequest) (*mkodriverv1.StandardResponse, error) {
	log.Printf("ConfigureOu: %+v", req)
	return &mkodriverv1.StandardResponse{
		Success: true,
	}, nil
}

func (s *fakeDriverServer) SubscribeOuCommands(
	req *mkodriverv1.Empty,
	stream mkodriverv1.MkoDriver_SubscribeOuCommandsServer,
) error {
	log.Printf("SubscribeOuCommands")
	return s.sendOuCommands(stream.Context(), func(event *mkodriverv1.OuCommandEvent) error {
		return stream.Send(event)
	})
}

func (s *fakeDriverServer) SubscribeOuCommandsForBoard(
	req *mkodriverv1.BoardRequest,
	stream mkodriverv1.MkoDriver_SubscribeOuCommandsForBoardServer,
) error {
	log.Printf("SubscribeOuCommandsForBoard: board=%s", req.GetBoardId())
	return s.sendOuCommands(stream.Context(), func(event *mkodriverv1.OuCommandEvent) error {
		return stream.Send(event)
	})
}

func (s *fakeDriverServer) sendOuCommands(ctx context.Context, send func(*mkodriverv1.OuCommandEvent) error) error {
	ticker := time.NewTicker(time.Second)
	defer ticker.Stop()

	for i := uint32(0); ; i++ {
		select {
		case <-ctx.Done():
			return ctx.Err()

		case <-ticker.C:
			err := send(&mkodriverv1.OuCommandEvent{
				CmdWord:        0x1000 + i,
				ResultWord:     0,
				ReceiveFromOu:  i%2 == 0,
				OuAddress:      1,
				Subaddress:     i % 31,
				WordCount:      4,
				DecodedCommand: "fake OU command",
				DecodedResult:  "fake OU command ok",
			})
			if err != nil {
				return err
			}
		}
	}
}

func (s *fakeDriverServer) ReadOuSubaddress(ctx context.Context, req *mkodriverv1.ReadOuSubaddressRequest) (*mkodriverv1.OuSubaddressData, error) {
	log.Printf("ReadOuSubaddress: %+v", req)
	return &mkodriverv1.OuSubaddressData{
		Success:       true,
		Subaddress:    req.GetSubaddress(),
		Sd:            []uint32{1, 2, 3, 4},
		CmdWord:       0x1200 + req.GetSubaddress(),
		ResultWord:    0,
		DecodedResult: "fake read OU subaddress ok",
	}, nil
}

func (s *fakeDriverServer) WriteOuSubaddress(ctx context.Context, req *mkodriverv1.WriteOuSubaddressRequest) (*mkodriverv1.StandardResponse, error) {
	log.Printf("WriteOuSubaddress: %+v", req)
	return &mkodriverv1.StandardResponse{
		Success: true,
	}, nil
}

func (s *fakeDriverServer) SetOuResponseWord(ctx context.Context, req *mkodriverv1.SetOuResponseWordRequest) (*mkodriverv1.StandardResponse, error) {
	log.Printf("SetOuResponseWord: %+v", req)
	return &mkodriverv1.StandardResponse{
		Success: true,
	}, nil
}

func (s *fakeDriverServer) WriteMemory(ctx context.Context, req *mkodriverv1.WriteMemoryRequest) (*mkodriverv1.StandardResponse, error) {
	log.Printf("WriteMemory: %+v", req)
	return &mkodriverv1.StandardResponse{
		Success: true,
	}, nil
}

func (s *fakeDriverServer) ReadMemory(ctx context.Context, req *mkodriverv1.ReadMemoryRequest) (*mkodriverv1.ReadMemoryResponse, error) {
	log.Printf("ReadMemory: %+v", req)
	return &mkodriverv1.ReadMemoryResponse{
		Data: []byte{0x01, 0x02, 0x03, 0x04},
	}, nil
}

func (s *fakeDriverServer) GetVersion(ctx context.Context, req *emptypb.Empty) (*mkodriverv1.VersionInfo, error) {
	log.Printf("GetVersion")
	return &mkodriverv1.VersionInfo{
		Version: "fake-mko-driver 0.1.0",
	}, nil
}

func main() {
	lis, err := net.Listen("tcp", ":50052")
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	grpcServer := grpc.NewServer()
	mkodriverv1.RegisterMkoDriverServer(grpcServer, &fakeDriverServer{})

	log.Printf("fake-mko-driver server listening at %v", lis.Addr())

	if err := grpcServer.Serve(lis); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
