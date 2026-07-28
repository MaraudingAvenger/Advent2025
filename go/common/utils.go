package utils

import (
	"os"
	"path/filepath"
)

func FindInputFile(path string) string {
	base, err := os.Getwd()
	if err != nil {
		panic(err)
	}

	if filepath.IsAbs(path) {
		if _, err := os.Stat(path); err == nil {
			return path
		}
		panic("file not found: " + path)
	}

	currentDir := base
	for {
		inputPath := filepath.Join(currentDir, path)
		if _, err := os.Stat(inputPath); err == nil {
			return inputPath
		}

		parent := filepath.Dir(currentDir)
		if parent == currentDir {
			panic("file not found: " + path)
		}

		currentDir = parent
	}
}
