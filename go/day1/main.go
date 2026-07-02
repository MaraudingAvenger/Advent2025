package day1

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

func Main() {
	input, err := os.ReadFile("/home/luke/dev/advent/2025/inputs/day1.txt")
	if err != nil {
		panic(err)
	}

	// the dial starts at 50, and goes from 0 to 99. We want to count how many times it passes 0.
	dial := uint32(50)
	passes := uint32(0)

	for _, line := range strings.Fields(string(input)) {
		// each line is something like "R15", "L5", etc.
		// get the direction
		if len(line) < 2 {
			continue
		}

		direction := line[0]
		n, err := strconv.ParseUint(line[1:], 10, 32)
		if err != nil {
			panic(err)
		}
		distance := uint32(n)

		// calculate the number of times the dial passes 0
		switch direction {
		case 'R':
			dial += distance
			if dial >= 100 {
				passes += dial / 100
				dial = dial % 100
			}
		case 'L':
			if distance > dial {
				passes += (distance - dial + 99) / 100
			}
			dial = (dial + 100 - (distance % 100)) % 100
		default:
			panic(fmt.Sprintf("invalid direction: %q", direction))
		}
	}

	fmt.Printf("The dial passed 0 %d times\n", passes)
}
