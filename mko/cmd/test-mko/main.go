package main

import (
	"context"
	"fmt"
	"log"
	"time"

	mkoapiv1 "mko/api/gen"

	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
)

func main() {
	conn, err := grpc.NewClient("127.0.0.1:50051", grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		log.Fatalf("connect: %v", err)
	}
	defer conn.Close()
	client := mkoapiv1.NewMkoWorkstationServiceClient(conn)

	ctx, cancel := context.WithTimeout(context.Background(), 4*time.Second)
	defer cancel()

	// Test ConfigureKK
	_, err = client.ConfigureKK(ctx, &mkoapiv1.ConfigureKKRequest{
		BoardId: "board-1", Index: 1, Channel: 0, BusControl: 0,
		OuAddress: 5, OuRespWord: 0, VectorWord: 0, SelftestWord: 0,
		RemoteIp: "192.168.2.254", RemotePort: 4000, OperationId: "op-1",
	})
	if err != nil {
		log.Printf("ConfigureKK ERROR: %v", err)
	} else {
		fmt.Println("ConfigureKK: OK")
	}

	// Test ConfigureExchange
	_, err = client.ConfigureExchange(ctx, &mkoapiv1.ConfigureExchangeRequest{
		BoardId: "board-1", Format: 1, Ks1: 0x1000, Ks2: 0x2000,
		Sd: []uint32{1, 2, 3, 4}, OperationId: "op-1",
	})
	if err != nil {
		log.Printf("ConfigureExchange ERROR: %v", err)
	} else {
		fmt.Println("ConfigureExchange: OK")
	}

	// Test RunExchange
	_, err = client.RunExchange(ctx, &mkoapiv1.RunExchangeRequest{
		BoardId: "board-1", OperationId: "op-1",
	})
	if err != nil {
		log.Printf("RunExchange ERROR: %v", err)
	} else {
		fmt.Println("RunExchange: OK")
	}

	// Test SubscribeExchangeResults
	stream, err := client.SubscribeExchangeResults(ctx, &mkoapiv1.SubscribeExchangeResultsRequest{BoardId: "board-1"})
	if err != nil {
		log.Printf("SubscribeExchangeResults ERROR: %v", err)
	} else {
		count := 0
		for {
			result, err := stream.Recv()
			if err != nil {
				fmt.Printf("ExchangeResults stream ended after %d events: %v\n", count, err)
				break
			}
			count++
			fmt.Printf("ExchangeResult #%d: format=%d ks1=0x%04X ks2=0x%04X result_word=%d decoded_result=%q\n",
				count, result.Format, result.Ks1, result.Ks2, result.ResultWord, result.DecodedResult)
			if count >= 3 {
				cancel()
				break
			}
		}
	}
}
