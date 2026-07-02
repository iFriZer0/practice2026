package transport

// В этом файле нет реализаций методов интерфейса, они в отедльном файлике рядом *-server.go

import (
	mkoapiv1 "mko/api/gen"
)

// Общая структура сервера
type MKOServer struct {
	mkoapiv1.UnimplementedMkoWorkstationServiceServer

	ouServer OuServerInterface
}

func NewMKOServer(ouServer OuServerInterface) *MKOServer {
	return &MKOServer{
		ouServer: ouServer,
	}
}
