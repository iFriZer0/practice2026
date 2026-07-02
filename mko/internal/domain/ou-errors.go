package domain

import (
	"errors"
	"time"
)

// ============================================================
// Конфигурация ОУ
// ============================================================

// OuConfig — конфигурация для ConfigureOu. Уже, чем MkoConfig из
// domain_common.go, потому что в режиме ОУ драйвер сам выставляет
// mode=0, busControl=0 внутри — эти поля оператору не нужны.
type OuConfig struct {
	Board        BoardID
	Index        McoIndex
	Channel      Channel
	OuAddress    int32
	ResponseWord int32
	RemoteIP     string
	RemotePort   int32
}

// ============================================================
// Подадреса ОУ
// ============================================================

// OuArea — область подадреса: приём (ПК читает то, что положил
// внешний КК) или передача (ПК заранее кладёт данные, которые ОУ
// отдаст по следующему запросу КК). WriteOuSubaddress у драйвера
// всегда пишет только в область передачи — отдельного поля area
// там нет, это не ошибка, а особенность контракта.
type OuArea bool

const (
	AreaReceive  OuArea = true  // receive_area = true
	AreaTransmit OuArea = false // receive_area = false
)

// OuSubaddressData — результат ReadOuSubaddress. DecodedResult —
// готовая строка от драйвера, свой decode писать не требуется.
type OuSubaddressData struct {
	Board         BoardID
	Subaddress    uint32
	Area          OuArea
	Sd            []uint32
	CmdWord       uint32
	ResultWord    uint32
	DecodedResult string
}

// ============================================================
// Поток входящих команд ОУ
// ============================================================

// OuCommandEvent — событие из SubscribeOuCommandsForBoard.
// DecodedCommand/DecodedResult уже готовы от драйвера.
type OuCommandEvent struct {
	Board          BoardID
	CmdWord        uint32
	ResultWord     uint32
	ReceiveFromOu  bool
	OuAddress      uint32
	Subaddress     uint32
	WordCount      uint32
	DecodedCommand string
	DecodedResult  string
	ReceivedAt     time.Time
}

// CommandWord / ResponseWordFlags — опциональные структурированные
// представления cmd_word/result_word. НЕ заполняются автоматически
// драйвером (он отдаёт готовые строки в DecodedCommand/DecodedResult
// выше) — держим про запас, если понадобится собственный парсинг
// вместо строк. Декодер пишется по Табл. п.10.3 и Табл. 3.10
// инструкции МКОИ.
type CommandWord struct {
	Raw        uint32
	OuAddress  uint32
	IsTransmit bool
	Subaddress uint32
	WordCount  uint32
}

type ResponseWordFlags struct {
	Raw  uint32
	TF   bool // неисправность ОУ
	SSF  bool // неисправность абонента
	SSB  bool // абонент занят (BUSY)
	SRQ  bool // запрос на обслуживание
	ENGC bool // разрешение групповых команд
}

// ============================================================
// Ошибки домена
// ============================================================
//
// Базовые ошибки — sentinel-значения через errors.New. Конкретные
// ошибки создаются через fmt.Errorf("...: %w", ErrXxx) — без
// собственного типа, errors.Is прекрасно работает и с обычным
// оборачиванием через %w. Service-слой классифицирует через
// errors.Is(err, domain.ErrXxx) и маппит в gRPC status.Code:
//   ErrValidation        -> codes.InvalidArgument
//   ErrDeviceRejected    -> codes.FailedPrecondition
//   ErrTimeout           -> codes.DeadlineExceeded
//   ErrDriverUnavailable -> codes.Unavailable
//   ErrNotImplemented    -> codes.Unimplemented
// Всё, что не завёрнуто ни в одну из них — codes.Internal.

var (
	ErrValidation        = errors.New("validation failed")
	ErrDeviceRejected    = errors.New("device rejected operation")
	ErrTimeout           = errors.New("timeout waiting for driver/device")
	ErrDriverUnavailable = errors.New("driver unavailable")
	ErrNotImplemented    = errors.New("not implemented by driver")
)
