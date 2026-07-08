// Package domain содержит типы, общие для service и transport
// слоёв. Ничего не знает ни про один из двух gRPC-контрактов.
//
// Файлы пакета разложены по доменам:
//
//	domain_common.go — общее для ОУ и КК (этот файл)
//	domain_ou.go      — специфика ОУ + модель ошибок (вы)
//	domain_kk.go       — специфика КК (напарник)
package domain

// BoardID — идентификатор конкретной платы МКО. Драйвер рассчитан
// на несколько плат одновременно, поэтому BoardID есть почти во
// всех операциях и ОУ, и КК.
type BoardID string

// McoIndex — номер контроллера МКО (1 или 2). В драйвере index<=0
// автоматически приводится к 1 — эту особенность не стоит повторять
// молча у себя, лучше валидировать на входе явно.
type McoIndex int32

type Channel int32

const (
	ChannelMain   Channel = 0 // основной
	ChannelBackup Channel = 1 // резервный
)

type ControllerMode int32

const (
	ModeOu ControllerMode = 0 // оконечное устройство
	ModeKk ControllerMode = 1 // контроллер канала
)

type BusControl int32

const (
	BusControlSoftware BusControl = 0
	BusControlHardware BusControl = 1
)

// MkoConfig — конфигурация через ConfigureMko (используется в
// основном КК-сценарием; для ОУ есть более узкий OuConfig в
// domain_ou.go).
type MkoConfig struct {
	Board        BoardID
	Index        McoIndex
	Mode         ControllerMode
	Channel      Channel
	BusControl   BusControl
	OuAddress    int32
	OuRespWord   int32
	VectorWord   int32
	SelftestWord int32
	RemoteIP     string
	RemotePort   int32
}
