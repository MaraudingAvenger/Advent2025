package main

import (
	"fmt"
	"strconv"
	"strings"

	utils "github.com/MaraudingAvenger/Advent2025/common"
)

func main() {
	lines, err := utils.ReadInputFile("inputs/day1.txt")
	if err != nil {
		panic(err)
	}

	// the dial starts at 50, and goes from 0 to 99. We want to count how many times it passes 0.
	dial := 50
	passes := 0

	const SIZE = 100

	for _, line := range strings.Fields(lines) {

		direction := line[0]
		n, err := strconv.Atoi(line[1:])
		if err != nil {
			panic(err)
		}
		distance := n

		// calculate the number of times the dial passes 0
		switch direction {
		case 'R':
			dial = (dial + distance) % SIZE
		case 'L':
			dial = (dial + SIZE - (distance % SIZE)) % SIZE
		default:
			panic(fmt.Sprintf("invalid direction: %q", direction))
		}

		if dial == 0 {
			passes++
		}
	}

	fmt.Printf("Part 1: %d\n", passes)

	dial = 50
	passes = 0
	for _, line := range strings.Fields(lines) {
		direction := line[0]
		n, err := strconv.Atoi(line[1:])
		if err != nil {
			panic(err)
		}
		distance := n

		switch direction {
		case 'R':
			for i := 0; i < distance; i++ {
				dial = (dial + 1) % SIZE
				if dial == 0 {
					passes++
				}
			}
		case 'L':
			for i := 0; i < distance; i++ {
				dial = (dial + SIZE - 1) % SIZE
				if dial == 0 {
					passes++
				}
			}
		default:
			panic(fmt.Sprintf("invalid direction: %q", direction))
		}
	}
	fmt.Printf("Part 2: %d\n", passes)
}
