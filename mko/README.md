neo programming # mko-service

gRPC-микросервис для работы с платой МКО (Мультиплексный Канал Обмена, ГОСТ Р 52070 / MIL-STD-1553) в режимах КК и ОУ.




## Содержание

- [Структура проекта](#структура-проекта)
- [Сборка на чистой машине](#сборка-на-чистой-машине)
  - [Шаг 1. Установить Go](#шаг-1-установить-go)
  - [Шаг 2. Установить protoc](#шаг-2-установить-protoc)
  - [Шаг 3. Установить protoc-плагины для Go](#шаг-3-установить-protoc-плагины-для-go)
  - [Шаг 4. Клонировать репозиторий](#шаг-4-клонировать-репозиторий)
  - [Шаг 5. Скачать зависимости Go-модуля](#шаг-5-скачать-зависимости-go-модуля)
  - [Шаг 6. Сгенерировать код из .proto](#шаг-6-сгенерировать-код-из-proto)
  - [Шаг 7. Собрать бинарники](#шаг-7-собрать-бинарники)
- [Конфигурация](#конфигурация)
- [Запуск](#запуск)
- [Справка по Makefile](#справка-по-makefile)

---

## Структура проекта

```
mko/
├── api/                        # Публичный .proto-контракт сервиса
│   ├── mko.proto
│   └── gen/                    # Сгенерированный Go-код (не редактировать вручную)
├── cmd/
│   ├── mko-service/            # Точка входа основного сервиса
│   └── fake-mko-driver/        # Заглушка драйвера для локального тестирования
├── config/
│   └── config.yaml             # Конфигурационный файл (хост и порт сервиса)
├── internal/
│   ├── config/                 # Загрузка конфига из YAML
│   ├── domain/                 # Доменные типы и ошибки
│   ├── logging/                # Логирующие декораторы сервисов
│   ├── service/                # Бизнес-логика (КК и ОУ)
│   └── transport/              # gRPC-серверы и клиент драйвера
├── bin/                        # Собранные бинарники (создаётся при сборке)
├── go.mod
├── go.sum
└── makefile
```

---

## Сборка на чистой машине

### Шаг 1. Установить Go

Требуемая версия: **Go 1.21 или новее** (проект использует `go 1.26.3`).

**Linux (рекомендуемый способ — официальный архив):**

```bash
# Скачать архив с официального сайта
wget https://go.dev/dl/go1.21.0.linux-amd64.tar.gz

# Распаковать в /usr/local
sudo tar -C /usr/local -xzf go1.21.0.linux-amd64.tar.gz

# Добавить Go в PATH — добавьте эти строки в ~/.bashrc или ~/.profile
export PATH=$PATH:/usr/local/go/bin
export GOPATH=$HOME/go
export PATH=$PATH:$GOPATH/bin

# Применить изменения в текущем терминале
source ~/.bashrc
```

**Проверить установку:**

```bash
go version
# Ожидаемый вывод: go version go1.21.x linux/amd64
```

**Arch Linux / Manjaro:**

```bash
sudo pacman -S go
```

**Ubuntu / Debian:**

```bash
sudo apt update && sudo apt install golang-go
```

> Если версия из репозитория дистрибутива старше 1.21 — используйте официальный архив выше.

---

### Шаг 2. Установить protoc

`protoc` — компилятор Protocol Buffers. Нужен для генерации Go-кода из `.proto`-файлов.

**Arch Linux / Manjaro:**

```bash
sudo pacman -S protobuf
```

**Ubuntu / Debian:**

```bash
sudo apt update && sudo apt install -y protobuf-compiler
```

**Проверить установку:**

```bash
protoc --version
# Ожидаемый вывод: libprotoc 3.x.x (или 4.x.x / 34.x)
```

---

### Шаг 3. Установить protoc-плагины для Go

Эти плагины вызываются компилятором `protoc` и генерируют Go-код из `.proto`-файлов.

```bash
go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest
```

Команды установят бинарники `protoc-gen-go` и `protoc-gen-go-grpc` в `$GOPATH/bin`.

**Убедитесь, что `$GOPATH/bin` находится в `PATH`** (если не добавили на шаге 1):

```bash
export PATH=$PATH:$(go env GOPATH)/bin
```

**Проверить установку:**

```bash
protoc-gen-go --version
# protoc-gen-go v1.36.x

protoc-gen-go-grpc --version
# protoc-gen-go-grpc 1.x.x
```

Тот же результат через Makefile:

```bash
make install-proto-plugins
```

---

### Шаг 4. Клонировать репозиторий

```bash
git clone <url-репозитория>
cd <папка-репозитория>/mko
```

Все последующие команды выполняются из директории `mko/`.

---

### Шаг 5. Скачать зависимости Go-модуля

```bash
make install
```

Команда последовательно выполняет:
1. `go mod tidy` — синхронизирует `go.mod` и `go.sum` с реальным кодом.
2. `go mod download` — скачивает все зависимости в локальный кэш модулей.

Зависимости проекта:

| Пакет | Назначение |
|-------|-----------|
| `google.golang.org/grpc` | gRPC runtime |
| `google.golang.org/protobuf` | Protocol Buffers runtime |
| `go.uber.org/zap` | Структурированное логирование |
| `gopkg.in/yaml.v3` | Чтение YAML-конфига |

> Если нет доступа в интернет — скопируйте кэш `$GOPATH/pkg/mod` с машины, где зависимости уже скачаны.

---

### Шаг 6. Сгенерировать код из .proto

Сгенерированные файлы уже лежат в репозитории. Этот шаг нужен только если вы изменили `.proto`-файлы.

**Сгенерировать всё:**

```bash
make proto
```

Одна команда генерирует три набора файлов из двух `.proto`-источников:

| Источник | Результат | Потребитель |
|----------|-----------|-------------|
| `api/mko.proto` | `api/gen/` (Go) | mko-service |
| `internal/.../mko_driver.proto` | `.../gen/mkodriverv1/` (Go) | клиент драйвера |
| `api/mko.proto` | `../gui/src/generated/mko/` (C++) | GUI |

**Или по отдельности:**

```bash
# Go-код публичного API (api/mko.proto → api/gen/)
make proto-api

# Go-код протокола драйвера
make proto-driver

# C++ / gRPC-код для GUI (api/mko.proto → gui/src/generated/mko/)
make proto-gui-cpp
```

Сгенерированные файлы не редактируются вручную. Если изменили `api/mko.proto` — запустите `make proto`, это обновит сразу и Go, и C++ файлы.

---

### Шаг 7. Собрать бинарники

**Собрать всё:**

```bash
make build
```

**Или по отдельности:**

```bash
# Основной микросервис
make service

# Заглушка драйвера для тестов
make fake-driver
```

Бинарники появятся в директории `bin/`:

```
bin/
├── mko-service       # Основной gRPC-сервис
└── fake-mko-driver   # Тестовая заглушка драйвера
```

---

## Конфигурация

Конфигурация читается из файла `config/config.yaml`:

```yaml
service:
  host: 127.0.0.1   # Адрес, на котором слушает mko-service
  port: 50051        # Порт mko-service

driver:
  host: 127.0.0.1   # Адрес mko-driver (реального или fake)
  port: 50052        # Порт mko-driver
```

**Переопределение через переменные окружения** (приоритет выше файла):

| Переменная | Что переопределяет |
|------------|-------------------|
| `CONFIG_PATH` | Путь к YAML-файлу конфигурации |
| `MKO_SERVICE_HOST` | Хост сервиса |
| `MKO_SERVICE_PORT` | Порт сервиса |
| `MKO_DRIVER_HOST` | Хост драйвера |
| `MKO_DRIVER_PORT` | Порт драйвера |

Пример запуска с переопределением:

```bash
MKO_SERVICE_PORT=9090 ./bin/mko-service
```

---

## Запуск

Перед запуском основного сервиса нужно запустить драйвер. В тестовой среде используется заглушка.

**Терминал 1 — запустить заглушку драйвера:**

```bash
make run-fake
# или напрямую:
./bin/fake-mko-driver
```

Ожидаемый вывод:
```
fake-mko-driver server listening at [::]:50052
```

**Терминал 2 — запустить основной сервис:**

```bash
make run
# или напрямую:
./bin/mko-service
```

Ожидаемый вывод:
```
mko-service started  {"service_addr": "127.0.0.1:50051", "driver_addr": "127.0.0.1:50052"}
```

Сервис принимает gRPC-запросы на `127.0.0.1:50051`.

**Остановка:** `Ctrl+C` — сервис завершается gracefully (дожидается окончания текущих запросов).

---

## Справка по Makefile

```bash
make help
```

Вывод:

```
  build                   Собрать все бинарники
  service                 Собрать mko-service
  fake-driver             Собрать fake-mko-driver
  install                 Скачать зависимости Go-модуля (go mod download)
  install-proto-plugins   Установить protoc-плагины для Go
  tidy                    Привести go.mod / go.sum в порядок
  proto                   Сгенерировать код из всех .proto (Go + C++ для GUI)
  proto-api               Сгенерировать Go-код из api/mko.proto
  proto-driver            Сгенерировать Go-код из proto драйвера
  proto-gui-cpp           Сгенерировать C++ / gRPC-код для GUI
  vet                     Статический анализ (go vet)
  lint                    Статический анализ + форматирование (gofmt -l)
  run                     Собрать и запустить mko-service
  run-fake                Собрать и запустить fake-mko-driver
  clean                   Удалить директорию bin/
  help                    Показать список целей с описаниями
```
