package domain

import (
	"errors"
)

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
