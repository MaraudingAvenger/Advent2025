package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"strings"
)

func FindRepeatedPatterns(n uint64) bool {
	n_Digits := int(math.Log10(float64(n))) + 1

	for i := 1; i <= n_Digits/2; i++ {
		pattern := n % uint64(math.Pow10(i))

		if n_Digits%i != 0 {
			continue
		}

		repeated := true
		for j := 1; j < n_Digits/i; j++ {
			if n%uint64(math.Pow10((j+1)*i))/uint64(math.Pow10(j*i)) != pattern {
				repeated = false
				break
			}
		}
		if repeated {
			return true
		}
	}
	return false
}

func main() {
	input, err := os.ReadFile("/home/luke/dev/advent/2025/inputs/day2.txt")
	if err != nil {
		panic(err)
	}

	lines := strings.TrimSpace(string(input))

	var invalid uint64 = 0

	for _, line := range strings.Fields(lines) {
		ranges := strings.Split(line, ",")

		for _, r := range ranges {
			parts := strings.Split(r, "-")
			low, _ := strconv.ParseUint(parts[0], 10, 64)
			high, _ := strconv.ParseUint(parts[1], 10, 64)

			for i := low; i <= high; i++ {
				n_Digits := int(math.Log10(float64(i))) + 1
				if n_Digits%2 == 1 {
					continue
				}
				firstHalf := i / uint64(math.Pow10(n_Digits/2))
				secondHalf := i % uint64(math.Pow10(n_Digits/2))
				if firstHalf == secondHalf {
					invalid += i
				}
			}
		}

		fmt.Printf("Part 1: %d\n", invalid)

		invalid = 0
		for _, r := range ranges {
			parts := strings.Split(r, "-")
			low, _ := strconv.ParseUint(parts[0], 10, 64)
			high, _ := strconv.ParseUint(parts[1], 10, 64)

			for i := low; i <= high; i++ {
				if FindRepeatedPatterns(i) {
					invalid += i
				}
			}
		}
		fmt.Printf("Part 2: %d\n", invalid)
	}
}
