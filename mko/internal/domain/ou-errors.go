package domain

import (
	"errors"
	"fmt"
)

var (
	ErrValidation        = errors.New("validation failed")
	ErrDeviceRejected    = errors.New("device rejected operation")
	ErrTimeout           = errors.New("timeout waiting for driver/device")
	ErrDriverUnavailable = errors.New("driver unavailable")
	ErrNotImplemented    = errors.New("not implemented by driver")
)

// NewValidationError — например: domain.NewValidationError("subaddress", "must be in 0..30")
func NewValidationError(field, message string) error {
	return fmt.Errorf("%s: %s: %w", field, message, ErrValidation)
}

// WrapDeviceRejected, WrapTimeout, WrapDriverUnavailable, WrapNotImplemented —
// добавляют текст поверх sentinel-а через %w, без собственного типа.
func WrapDeviceRejected(message string) error {
	return fmt.Errorf("%s: %w", message, ErrDeviceRejected)
}

func WrapTimeout(message string) error {
	return fmt.Errorf("%s: %w", message, ErrTimeout)
}

func WrapDriverUnavailable(message string) error {
	return fmt.Errorf("%s: %w", message, ErrDriverUnavailable)
}

func WrapNotImplemented(message string) error {
	return fmt.Errorf("%s: %w", message, ErrNotImplemented)
}
