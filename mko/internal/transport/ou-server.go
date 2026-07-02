package transport

// Файл реализации интерфейсов

import (
	"context"
	mkoapiv1 "mko/api/gen"

	"google.golang.org/grpc"
)

type OuServerInterface interface {
}

func (M MKOServer) GetStatus(ctx context.Context, empty *mkoapiv1.Empty) (*mkoapiv1.StatusResponse, error) {
	//TODO implement me
	panic("implement me")
}

func (M MKOServer) ConfigureOu(ctx context.Context, request *mkoapiv1.ConfigureOuRequest) (*mkoapiv1.Empty, error) {
	//TODO implement me
	panic("implement me")
}

func (M MKOServer) SetOuResponseWord(ctx context.Context, request *mkoapiv1.SetOuResponseWordRequest) (*mkoapiv1.Empty, error) {
	//TODO implement me
	panic("implement me")
}

func (M MKOServer) ReadOuSubaddress(ctx context.Context, request *mkoapiv1.ReadOuSubaddressRequest) (*mkoapiv1.OuSubaddressData, error) {
	//TODO implement me
	panic("implement me")
}

func (M MKOServer) WriteOuSubaddress(ctx context.Context, request *mkoapiv1.WriteOuSubaddressRequest) (*mkoapiv1.Empty, error) {
	//TODO implement me
	panic("implement me")
}

func (M MKOServer) SendRawOuData(ctx context.Context, request *mkoapiv1.WriteOuSubaddressRequest) (*mkoapiv1.Empty, error) {
	//TODO implement me
	panic("implement me")
}

func (M MKOServer) SubscribeOuCommands(request *mkoapiv1.SubscribeOuCommandsRequest, g grpc.ServerStreamingServer[*mkoapiv1.OuCommandEvent]) error {
	//TODO implement me
	panic("implement me")
}

func (M MKOServer) ClearReceiveBuffer(ctx context.Context, request *mkoapiv1.ClearBufferRequest) (*mkoapiv1.Empty, error) {
	//TODO implement me
	panic("implement me")
}

func (M MKOServer) ClearTransmitBuffer(ctx context.Context, request *mkoapiv1.ClearBufferRequest) (*mkoapiv1.Empty, error) {
	//TODO implement me
	panic("implement me")
}

func (M MKOServer) mustEmbedUnimplementedMkoWorkstationServiceServer() {
	//TODO implement me
	panic("implement me")
}
