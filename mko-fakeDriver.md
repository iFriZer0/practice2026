# Отладка MKO через fake-driver

Документ описывает локальную отладку полной цепочки:

```text
Qt GUI -> MKO microservice -> fake mko_driver
```

Fake-driver нужен, чтобы проверять GUI и микросервис без реального драйвера и платы.

## Схема

```text
Qt GUI
  gRPC: 127.0.0.1:50051
        |
        v
MKO microservice
  gRPC client to driver: 127.0.0.1:50052
        |
        v
fake-mko-driver
```

Текущие адреса:

- GUI подключается к MKO microservice по `127.0.0.1:50051`.
- fake-driver слушает `:50052`.
- MKO microservice должен быть настроен так, чтобы ходить к драйверу по `127.0.0.1:50052`.

## Что проверяет fake-driver

Fake-driver реализует gRPC service `mko.driver.v1.MkoDriver` из:

```text
mko/internal/transport/clients/mko-driver-service/mko_driver.proto
```

Он принимает команды, пишет их в лог и возвращает успешные ответы.

Для KK сейчас важны:

- `ConfigureMko`
- `ConfigureExchange`
- `SendCommandRun`
- `SubscribeExchangeResultsForBoard`

Для OU:

- `ConfigureOu`
- `ReadOuSubaddress`
- `WriteOuSubaddress`
- `SetOuResponseWord`
- `SubscribeOuCommandsForBoard`

Также реализованы вспомогательные методы:

- `ReadMemory`
- `WriteMemory`
- `GetVersion`
- `ReadExchangeConfig`

## Запуск fake-driver

Из корня Go-модуля `mko`:

```bash
cd mko
go run ./cmd/fake-mko-driver
```

Ожидаемый лог:

```text
fake-mko-driver server listening at [::]:50052
```

Если порт занят, либо останови процесс на `50052`, либо поменяй порт в:

```text
mko/cmd/fake-mko-driver/main.go
```

## Запуск MKO microservice

Микросервис должен слушать GUI на `127.0.0.1:50051` и подключаться к fake-driver на `127.0.0.1:50052`.

Адреса для локальной отладки лежат в:

```text
mko/config/local.env
```

Содержимое:

```bash
export MKO_SERVICE_ADDR=127.0.0.1:50051
export MKO_DRIVER_ADDR=127.0.0.1:50052
```

Запуск:

```bash
cd mko
source config/local.env
go run ./cmd/mko-service
```

Команда поднимает gRPC service `mko.workstation.v1.MkoWorkstationService` для GUI и создает gRPC client к fake-driver `mko.driver.v1.MkoDriver`.

## Запуск GUI

Если `gui/src/generated/mko` отсутствует, сначала сгенерируй C++ gRPC stubs из workstation proto:

```bash
cd mko
make proto-gui-cpp
```

Команда создает:

```text
gui/src/generated/mko/mko.pb.h
gui/src/generated/mko/mko.pb.cc
gui/src/generated/mko/mko.grpc.pb.h
gui/src/generated/mko/mko.grpc.pb.cc
```

Из папки GUI:

```bash
cd gui/src
./build.sh
open build/out/ui.app
```

Если `open` недоступен или не нужен, можно запустить бинарь напрямую:

```bash
./build/out/ui.app/Contents/MacOS/ui
```

## Проверка KK

1. Открой вкладку `МКО`.
2. В левой части вкладки заполни блок `КК: ConfigureKK`.
3. Нажми `Настроить КК`.
4. В логах fake-driver должен появиться вызов:

```text
ConfigureMko: ...
```

5. Заполни блок `КК: ConfigureExchange`.
6. Нажми `Настроить обмен`.
7. В логах fake-driver должен появиться вызов:

```text
ConfigureExchange: ...
```

8. Нажми `RUN: запустить обмен`.
9. В логах fake-driver должен появиться вызов:

```text
SendCommandRun: ...
```

Если в GUI появляется `успешно`, значит цепочка `GUI -> microservice -> fake-driver` работает для unary-команд.

## Проверка stream-результатов KK

Для проверки результатов обмена микросервис должен вызывать:

```text
SubscribeExchangeResultsForBoard
```

Fake-driver раз в секунду отправляет тестовый результат:

```text
format = 1
ks1 = 0x1000
ks2 = 0x2000
sd = [counter, 2, 3, 4]
answer_word_1 = 0xAAAA
answer_word_2 = 0xBBBB
result_word = 0
decoded_result = "fake exchange result ok"
```

Ожидаемый лог fake-driver:

```text
SubscribeExchangeResultsForBoard: board=<board_id>
```

## Проверка OU

В правой части вкладки `МКО` проверяются OU-команды.

Ожидаемые вызовы в fake-driver:

```text
ConfigureOu: ...
ReadOuSubaddress: ...
WriteOuSubaddress: ...
SetOuResponseWord: ...
SubscribeOuCommandsForBoard: board=<board_id>
```

`SubscribeOuCommandsForBoard` раз в секунду отправляет тестовое событие команды ОУ:

```text
cmd_word = 0x1000 + counter
result_word = 0
receive_from_ou = true/false
ou_address = 1
subaddress = counter % 31
word_count = 4
decoded_command = "fake OU command"
decoded_result = "fake OU command ok"
```

## Что смотреть при ошибках

Если GUI пишет ошибку подключения:

- Проверь, что MKO microservice запущен на `127.0.0.1:50051`.
- Проверь, что адрес в GUI совпадает с `DEFAULT_MKO_SERVICE_ADDRESS` в `gui/src/graphical_views/views/main/qt_view_mko.h`.

Если микросервис пишет ошибку подключения к драйверу:

- Проверь, что fake-driver запущен.
- Проверь, что fake-driver слушает `:50052`.
- Проверь, что микросервис подключается к `127.0.0.1:50052`.

Если fake-driver не показывает вызов:

- Значит запрос не дошел до драйверного клиента.
- Проверяй по цепочке: GUI log -> MKO microservice transport -> service/usecase -> mko-driver-client.

## Минимальный успешный сценарий

Три терминала:

```bash
# terminal 1
cd mko
go run ./cmd/fake-mko-driver
```

```bash
# terminal 2
cd mko
source config/local.env
go run ./cmd/mko-service
```

```bash
# terminal 3
cd mko
make proto-gui-cpp

cd gui/src
./build.sh
./build/out/ui.app/Contents/MacOS/ui
```

Дальше в GUI:

1. `Настроить КК`
2. `Настроить обмен`
3. `RUN: запустить обмен`

В fake-driver должны появиться:

```text
ConfigureMko
ConfigureExchange
SendCommandRun
```
