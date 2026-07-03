package domain

import "errors"

var (
	ErrInvalidKKConfig       = errors.New("invalid kk config")
	ErrInvalidExchangeConfig = errors.New("invalid exchange config")
	ErrUnsupportedFormat     = errors.New("unsupported format")
	ErrInvalidDataWordsCount = errors.New("invalid data words count")
	ErrExchangeNotConfigured = errors.New("exchange is not configured")
	ErrKKNotConfigured       = errors.New("kk is not configured")
	ErrExchangeFailed        = errors.New("exchange failed")
	ErrOperationCanceled     = errors.New("operation canceled")
)
