MODULE      := mko
BIN_DIR     := bin
GUI_CPP_OUT := ../gui/src/generated/mko

SERVICE     := $(BIN_DIR)/mko-service
FAKE_DRIVER := $(BIN_DIR)/fake-mko-driver

# go install кладёт бинарники сюда; добавляем в PATH чтобы protoc их нашёл
GOPATH      := $(shell go env GOPATH)
export PATH := $(GOPATH)/bin:$(PATH)

# ldflags: убираем отладочную информацию и таблицу символов из релизного бинаря
LDFLAGS     := -ldflags "-s -w"

# ────────────────────────────────────────────────────────────────
.PHONY: all build service fake-driver \
        proto proto-api proto-driver proto-gui-cpp \
        mock \
        install install-proto-plugins \
        tidy lint vet test \
        run run-fake \
        clean help

# ────────────────────────────────────────────────────────────────
## Цель по умолчанию
all: build

# ────────────────────────────────────────────────────────────────
## Сборка

build: service fake-driver  ## Собрать все бинарники

service:                     ## Собрать mko-service
	go build $(LDFLAGS) -o $(SERVICE) ./cmd/mko-service

fake-driver:                 ## Собрать fake-mko-driver
	go build $(LDFLAGS) -o $(FAKE_DRIVER) ./cmd/fake-mko-driver

# ────────────────────────────────────────────────────────────────
## Зависимости

install: tidy                ## Скачать зависимости Go-модуля (go mod download)
	go mod download

install-proto-plugins:       ## Установить protoc-плагины для Go (protoc-gen-go, protoc-gen-go-grpc)
	go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
	go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest

tidy:                        ## Привести go.mod / go.sum в порядок (go mod tidy)
	go mod tidy

# ────────────────────────────────────────────────────────────────
## Кодогенерация из .proto

proto: proto-api proto-driver proto-gui-cpp  ## Сгенерировать код из всех .proto (Go + C++ для GUI)

proto-api:                     ## Сгенерировать Go-код из api/mko.proto
	protoc \
		--proto_path=api \
		--go_out=. --go_opt=module=$(MODULE) \
		--go-grpc_out=. --go-grpc_opt=module=$(MODULE) \
		api/mko.proto

proto-driver:                  ## Сгенерировать Go-код из proto драйвера
	protoc \
		--proto_path=internal/transport/clients/mko-driver-service \
		--go_out=. --go_opt=module=$(MODULE) \
		--go-grpc_out=. --go-grpc_opt=module=$(MODULE) \
		internal/transport/clients/mko-driver-service/mko_driver.proto

proto-gui-cpp:                 ## Сгенерировать C++ / gRPC-код из api/mko.proto → gui/src/generated/mko/
	mkdir -p $(GUI_CPP_OUT)
	protoc \
		--proto_path=api \
		--cpp_out=$(GUI_CPP_OUT) \
		--grpc_out=$(GUI_CPP_OUT) \
		--plugin=protoc-gen-grpc=$$(which grpc_cpp_plugin) \
		api/mko.proto

# ────────────────────────────────────────────────────────────────
## Генерация моков

mock:                          ## Сгенерировать моки для тестов (go.uber.org/mock/mockgen)
	go run go.uber.org/mock/mockgen \
		-source=internal/service/ou-usecase.go \
		-destination=internal/service/mocks/driver_ou_client_mock.go \
		-package=mocks
	go run go.uber.org/mock/mockgen \
		-source=internal/service/kk-service.go \
		-destination=internal/service/mocks/kk_driver_mock.go \
		-package=mocks
	go run go.uber.org/mock/mockgen \
		-source=internal/transport/ou-server.go \
		-destination=internal/transport/mocks/ou_service_mock.go \
		-package=mocks
	go run go.uber.org/mock/mockgen \
		-source=internal/transport/kk-server.go \
		-destination=internal/transport/mocks/kk_service_mock.go \
		-package=mocks

# ────────────────────────────────────────────────────────────────
## Проверка кода

test: mock                   ## Запустить все тесты (предварительно генерирует моки)
	go test ./...

vet:                         ## Статический анализ (go vet)
	go vet ./...

lint: vet                    ## Статический анализ + форматирование (gofmt -l)
	@unformatted=$$(gofmt -l .); \
	if [ -n "$$unformatted" ]; then \
		echo "Следующие файлы не отформатированы (запустите gofmt -w .):" ; \
		echo "$$unformatted" ; \
		exit 1 ; \
	fi

# ────────────────────────────────────────────────────────────────
## Запуск

run: service                 ## Собрать и запустить mko-service
	$(SERVICE)

run-fake: fake-driver        ## Собрать и запустить fake-mko-driver
	$(FAKE_DRIVER)

# ────────────────────────────────────────────────────────────────
## Очистка

clean:                       ## Удалить директорию bin/
	rm -rf $(BIN_DIR)

# ────────────────────────────────────────────────────────────────
## Справка

help:                        ## Показать список целей с описаниями
	@grep -E '^[a-zA-Z_-]+:.*##' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*##"}; {printf "  %-22s %s\n", $$1, $$2}'
