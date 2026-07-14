package config

import (
	"fmt"
	"os"

	"gopkg.in/yaml.v3"
)

const defaultConfigPath = "config/config.yaml"

type Address struct {
	Host string `yaml:"host"`
	Port int    `yaml:"port"`
}

func (a Address) Addr() string {
	return fmt.Sprintf("%s:%d", a.Host, a.Port)
}

type Config struct {
	Service Address `yaml:"service"`
	Driver  Address `yaml:"driver"`
}

// Load читает YAML-конфиг из файла. Путь берётся из переменной окружения
// CONFIG_PATH; если она не задана, используется config/config.yaml
// относительно рабочей директории.
// Переменные MKO_SERVICE_ADDR / MKO_DRIVER_ADDR перезаписывают
// соответствующие адреса из файла — это позволяет менять адреса в CI
// без редактирования файла.
func Load() (*Config, error) {
	path := os.Getenv("CONFIG_PATH")
	if path == "" {
		path = defaultConfigPath
	}

	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("read config %s: %w", path, err)
	}

	var cfg Config
	if err := yaml.Unmarshal(data, &cfg); err != nil {
		return nil, fmt.Errorf("parse config %s: %w", path, err)
	}

	// Env-переменные имеют приоритет над файлом.
	if v := os.Getenv("MKO_SERVICE_HOST"); v != "" {
		cfg.Service.Host = v
	}
	if v := os.Getenv("MKO_SERVICE_PORT"); v != "" {
		if _, err := fmt.Sscanf(v, "%d", &cfg.Service.Port); err != nil {
			return nil, fmt.Errorf("invalid MKO_SERVICE_PORT: %w", err)
		}
	}
	if v := os.Getenv("MKO_DRIVER_HOST"); v != "" {
		cfg.Driver.Host = v
	}
	if v := os.Getenv("MKO_DRIVER_PORT"); v != "" {
		if _, err := fmt.Sscanf(v, "%d", &cfg.Driver.Port); err != nil {
			return nil, fmt.Errorf("invalid MKO_DRIVER_PORT: %w", err)
		}
	}

	return &cfg, nil
}