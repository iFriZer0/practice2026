package domain

type KKConfig struct {
	Index        int
	Channel      int
	BusControl   int
	OUAddress    int
	OURespWord   uint32
	VectorWord   uint32
	SelftestWord uint32
	RemoteIP     string
	RemotePort   int
	OperationID  string
	BoardID      string
}
type ExchangeConfig struct {
	Format      int32
	KS1         uint32
	KS2         uint32
	SD          []uint32
	OperationID string
	BoardID     string
}

type RunExchangeCommand struct {
	OperationID string
	BoardID     string
}

type ExchangeResult struct {
	Format       int32
	KS1          uint32
	KS2          uint32
	SD           []uint32
	AnswerWord1  uint32
	AnswerWord2  uint32
	DecodeResult string
	BoardID      uint32
}
