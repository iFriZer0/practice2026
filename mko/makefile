MODULE := mko

.PHONY: proto proto-api proto-driver tidy run

proto: proto-api proto-driver

proto-api:
	protoc \
		--proto_path=api \
		--go_out=. --go_opt=module=$(MODULE) \
		--go-grpc_out=. --go-grpc_opt=module=$(MODULE) \
		api/mko.proto

proto-driver:
	protoc \
		--proto_path=internal/transport/clients/mko-driver-service \
		--go_out=. --go_opt=module=$(MODULE) \
		--go-grpc_out=. --go-grpc_opt=module=$(MODULE) \
		internal/transport/clients/mko-driver-service/mko_driver.proto

tidy:
	go mod tidy

run:
	go run ./cmd/mko