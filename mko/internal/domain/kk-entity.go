package domain

// KKConfig — конфигурация контроллера канала. В mko_driver она
// отправляется через ConfigureMko с mode=1.
type KKConfig struct {
	Board        BoardID
	Index        McoIndex
	Channel      Channel
	BusControl   BusControl
	OuAddress    int32
	OuRespWord   int32
	VectorWord   int32
	SelftestWord int32
	RemoteIP     string
	RemotePort   int32
	OperationID  string
}

// ExchangeConfig — конфигурация обмена КК: формат, командные слова
// и слова данных.
type ExchangeConfig struct {
	Format      int32
	KS1         uint32
	KS2         uint32
	SD          []uint32
	OperationID string
	Board       BoardID
}

type RunExchangeCommand struct {
	OperationID string
	Board       BoardID
}

// ExchangeResult — событие/результат из SubscribeExchangeResults.
// DecodedResult приходит готовым от драйвера.
type ExchangeResult struct {
	Board         BoardID
	Format        int32
	KS1           uint32
	KS2           uint32
	SD            []uint32
	AnswerWord1   uint32
	AnswerWord2   uint32
	ResultWord    uint32
	DecodedResult string
}
