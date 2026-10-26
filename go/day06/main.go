package main

import (
	"fmt"
	"strconv"
	"strings"

	utils "github.com/MaraudingAvenger/Advent2025/common"
)

func main() {
	text, err := utils.ReadInputFile(utils.FindInputFile("inputs/day6.txt"))
	if err != nil {
		fmt.Println("Error reading input file:", err)
		return
	}

	lines := strings.Split(text, "\n")
	for i := range lines {
		lines[i] = strings.TrimSpace(lines[i])
	}

	// Part 1: Transpose and apply operators column-wise
	total := solvePart1(lines)
	fmt.Printf("Part 1: %d\n", total)

	// Part 2: Read vertically and apply operators
	total = solvePart2(lines)
	fmt.Printf("Part 2: %d\n", total)
}

func solvePart1(lines []string) int {
	// Skip last line (operators)
	dataLines := lines[:len(lines)-1]
	opLine := strings.Fields(strings.TrimSpace(lines[len(lines)-1]))

	// Transpose: group numbers by column index
	numColumns := len(strings.Fields(dataLines[0]))
	columns := make([][]int, numColumns)
	for i := range columns {
		columns[i] = make([]int, 0, len(dataLines))
	}

	for _, line := range dataLines {
		fields := strings.Fields(line)
		for i, field := range fields {
			num, _ := strconv.Atoi(field)
			columns[i] = append(columns[i], num)
		}
	}

	total := 0
	for i, op := range opLine {
		switch op {
		case "+":
			for _, num := range columns[i] {
				total += num
			}
		case "*":
			product := 1
			for _, num := range columns[i] {
				product *= num
			}
			total += product
		}
	}

	return total
}

func solvePart2(lines []string) int {
	// Remove trailing empty lines
	for len(lines) > 0 && lines[len(lines)-1] == "" {
		lines = lines[:len(lines)-1]
	}

	// Last line contains operators
	// opLine := strings.Fields(strings.TrimSpace(lines[len(lines)-1]))

	// Data lines (all lines except last)
	dataLines := lines[:len(lines)-1]

	// Find the maximum line length
	maxLen := 0
	for _, line := range dataLines {
		if len(line) > maxLen {
			maxLen = len(line)
		}
	}

	// Iterate from right to left, extracting numbers vertically
	total := 0
	var operands []int
	var ops []string

	for col := maxLen - 1; col >= 0; col-- {
		// Build vertical string with ONLY digits (skip operators)
		var numStr strings.Builder
		for _, line := range dataLines {
			if col < len(line) && line[col] >= '0' && line[col] <= '9' {
				numStr.WriteByte(line[col])
			}
		}

		trimmed := strings.TrimSpace(numStr.String())
		if trimmed == "" {
			// Column contains operators - pop and apply
			if len(ops) > 0 {
				op := ops[len(ops)-1]
				ops = ops[:len(ops)-1]
				applyOp(&total, op, operands)
				operands = nil
			}
		} else {
			// Column contains a number - accumulate
			num, err := strconv.Atoi(trimmed)
			if err == nil {
				operands = append(operands, num)
			}
		}
	}

	// Apply final operator
	if len(ops) > 0 {
		op := ops[len(ops)-1]
		ops = ops[:len(ops)-1]
		applyOp(&total, op, operands)
	}

	return total
}

func applyOp(total *int, op string, operands []int) {
	switch op {
	case "+":
		sum := 0
		for _, num := range operands {
			sum += num
		}
		*total += sum
	case "*":
		product := 1
		for _, num := range operands {
			product *= num
		}
		*total += product
	}
}
