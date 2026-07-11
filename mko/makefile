MODULE     := mko
BIN_DIR    := bin
GUI_CPP_OUT := ../gui/src/generated/mko

SERVICE     := $(BIN_DIR)/mko-service
FAKE_DRIVER := $(BIN_DIR)/fake-mko-driver

.PHONY: all build service fake-driver clean proto proto-api proto-driver proto-gui-cpp tidy run run-fake

all: build

build: service fake-driver

service:
	go build -o $(SERVICE) ./cmd/mko-service

fake-driver:
	go build -o $(FAKE_DRIVER) ./cmd/fake-mko-driver

clean:
	rm -rf $(BIN_DIR)

run: service
	$(SERVICE)

run-fake: fake-driver
	$(FAKE_DRIVER)

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
