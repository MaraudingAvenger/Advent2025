package main

import (
	"fmt"
	"strings"

	utils "github.com/MaraudingAvenger/Advent2025/common"
)

func FindLargestNumber(n string, d uint) uint64 {
	// Find the largest number of exactly d digits from n while preserving order.
	if len(n) < int(d) {
		panic(fmt.Sprintf("n must have at least %d digits", d))
	}
	if d == 0 {
		return 0
	}

	targetLen := int(d)
	toDrop := len(n) - targetLen
	stack := make([]byte, 0, len(n))

	for i := 0; i < len(n); i++ {
		c := n[i]
		if c < '0' || c > '9' {
			panic(fmt.Sprintf("n must contain only digits: found %q", c))
		}

		for toDrop > 0 && len(stack) > 0 && stack[len(stack)-1] < c {
			stack = stack[:len(stack)-1]
			toDrop--
		}

		stack = append(stack, c)
	}

	if toDrop > 0 {
		stack = stack[:len(stack)-toDrop]
	}

	selected := stack[:targetLen]

	var largest uint64
	for _, c := range selected {
		largest = largest*10 + uint64(c-'0')
	}

	return largest
}

func main() {
	lines, err := utils.ReadInputFile("inputs/day3.txt")
	if err != nil {
		panic(err)
	}

	var totalJolts uint64 = 0

	for _, line := range strings.Fields(lines) {
		line = strings.TrimSpace(line)
		if len(line) == 0 {
			continue
		}

		largest := FindLargestNumber(line, 2)
		totalJolts += largest
	}

	fmt.Printf("Part 1: %d\n", totalJolts)

	totalJolts = 0
	for _, line := range strings.Fields(lines) {
		line = strings.TrimSpace(line)
		if len(line) == 0 {
			continue
		}

		largest := FindLargestNumber(line, 12)
		totalJolts += largest
	}

	fmt.Printf("Part 2: %d\n", totalJolts)
}
