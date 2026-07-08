package main

import (
	"log"
	"log/slog"
	"net"
	"os"
	"os/signal"
	"syscall"

	mkoapiv1 "mko/api/gen"
	"mko/internal/service"
	"mko/internal/transport"
	mkodriver "mko/internal/transport/clients/mko-driver-service"
	mkodriverv1 "mko/internal/transport/clients/mko-driver-service/gen/mkodriverv1"

	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
)

const (
	defaultServiceAddr = "127.0.0.1:50051"
	defaultDriverAddr  = "127.0.0.1:50052"
)

func main() {
	serviceAddr := envOrDefault("MKO_SERVICE_ADDR", defaultServiceAddr)
	driverAddr := envOrDefault("MKO_DRIVER_ADDR", defaultDriverAddr)

	driverConn, err := grpc.NewClient(driverAddr, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		log.Fatalf("failed to create mko-driver client: %v", err)
	}
	defer driverConn.Close()

	driverGrpcClient := mkodriverv1.NewMkoDriverClient(driverConn)
	kkDriver := transport.NewMKODriverClient(driverGrpcClient)
	ouDriver := mkodriver.New(driverGrpcClient)

	kkService := service.NewKKService(kkDriver)
	ouService := service.NewOuService(ouDriver, slog.Default())
	mkoServer := transport.NewMKOServer(ouService, kkService)

	listener, err := net.Listen("tcp", serviceAddr)
	if err != nil {
		log.Fatalf("failed to listen on %s: %v", serviceAddr, err)
	}

	grpcServer := grpc.NewServer()
	mkoapiv1.RegisterMkoWorkstationServiceServer(grpcServer, mkoServer)

	go func() {
		log.Printf("mko-service listening on %s, driver=%s", serviceAddr, driverAddr)
		if err := grpcServer.Serve(listener); err != nil {
			log.Fatalf("failed to serve mko-service: %v", err)
		}
	}()

	waitForShutdown(grpcServer)
}

func envOrDefault(key, fallback string) string {
	value := os.Getenv(key)
	if value == "" {
		return fallback
	}
	return value
}

func waitForShutdown(server *grpc.Server) {
	stop := make(chan os.Signal, 1)
	signal.Notify(stop, os.Interrupt, syscall.SIGTERM)
	<-stop

	log.Print("stopping mko-service")
	server.GracefulStop()
}
