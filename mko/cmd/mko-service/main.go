package main

import (
	"log"
	"net"
	"os"
	"os/signal"
	"syscall"

	mkoapiv1 "mko/api/gen"
	"mko/internal/config"
	"mko/internal/logging"
	"mko/internal/service"
	"mko/internal/transport"
	mkodriver "mko/internal/transport/clients/mko-driver-service"
	mkodriverv1 "mko/internal/transport/clients/mko-driver-service/gen/mkodriverv1"

	"go.uber.org/zap"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
)

func main() {
	cfg, err := config.Load()
	if err != nil {
		log.Fatalf("failed to load config: %v", err)
	}

	serviceAddr := cfg.Service.Addr()
	driverAddr := cfg.Driver.Addr()

	logger, err := logging.NewZapLogger()
	if err != nil {
		log.Fatalf("failed to create logger: %v", err)
	}
	defer func() {
		_ = logger.Sync()
	}()

	driverConn, err := grpc.NewClient(driverAddr, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		logger.Fatal("failed to create mko-driver client", zap.Error(err), zap.String("driver_addr", driverAddr))
	}
	defer driverConn.Close()

	driverGrpcClient := mkodriverv1.NewMkoDriverClient(driverConn)
	kkDriver := transport.NewMKODriverClient(driverGrpcClient)
	ouDriver := mkodriver.New(driverGrpcClient)

	kkService := logging.NewKKService(service.NewKKService(kkDriver), logger)
	ouService := logging.NewOuService(service.NewOuService(ouDriver), logger)
	mkoServer := transport.NewMKOServer(ouService, kkService)

	listener, err := net.Listen("tcp", serviceAddr)
	if err != nil {
		logger.Fatal("failed to listen mko-service", zap.Error(err), zap.String("service_addr", serviceAddr))
	}

	grpcServer := grpc.NewServer()
	mkoapiv1.RegisterMkoWorkstationServiceServer(grpcServer, mkoServer)

	go func() {
		logger.Info("mko-service started", zap.String("service_addr", serviceAddr), zap.String("driver_addr", driverAddr))
		if err := grpcServer.Serve(listener); err != nil {
			logger.Fatal("failed to serve mko-service", zap.Error(err))
		}
	}()

	waitForShutdown(grpcServer, logger)
}

func waitForShutdown(server *grpc.Server, logger *zap.Logger) {
	stop := make(chan os.Signal, 1)
	signal.Notify(stop, os.Interrupt, syscall.SIGTERM)
	sig := <-stop

	logger.Info("stopping mko-service", zap.String("signal", sig.String()))
	server.GracefulStop()
}
