MODULE := mko
GUI_CPP_OUT := ../gui/src/generated/mko

.PHONY: proto proto-api proto-driver proto-gui-cpp tidy run

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

proto-gui-cpp:
	mkdir -p $(GUI_CPP_OUT)
	protoc \
		--proto_path=api \
		--cpp_out=$(GUI_CPP_OUT) \
		--grpc_out=$(GUI_CPP_OUT) \
		--plugin=protoc-gen-grpc=$$(which grpc_cpp_plugin) \
		api/mko.proto

tidy:
	go mod tidy

run:
	go run ./cmd/mko
