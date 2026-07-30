package main

import (
	"fmt"
	"sort"
	"strconv"
	"strings"

	utils "github.com/MaraudingAvenger/Advent2025/common"
)

type interval struct {
	start uint64
	end   uint64
}

func parseRange(line string) (interval, bool) {
	left, right, ok := strings.Cut(strings.TrimSpace(line), "-")
	if !ok {
		panic("invalid range format: " + line)
	}
	start, err1 := strconv.ParseUint(left, 10, 64)
	end, err2 := strconv.ParseUint(right, 10, 64)
	if err1 != nil || err2 != nil {
		return interval{}, false
	}
	return interval{start: start, end: end}, true
}

func main() {
	lines, err := utils.ReadInputFile(utils.FindInputFile("inputs/day5.txt"))
	if err != nil {
		fmt.Println("Error reading input file:", err)
		return
	}

	var ranges []interval
	var ingredients []uint64

	parsingRanges := true

	for _, line := range strings.Split(lines, "\n") {
		line = strings.TrimSpace(line)
		if line == "" {
			parsingRanges = false
			continue
		}

		if parsingRanges {
			r, ok := parseRange(line)
			if ok {
				ranges = append(ranges, r)
			}
		} else {
			if value, err := strconv.ParseUint(line, 10, 64); err == nil {
				ingredients = append(ingredients, value)
			}
		}
	}

	// Sort ranges and ingredients in place.
	sort.Slice(ranges, func(i, j int) bool {
		return ranges[i].start < ranges[j].start
	})
	sort.Slice(ingredients, func(i, j int) bool {
		return ingredients[i] < ingredients[j]
	})

	// Merge ranges in place.
	mergedCount := 0
	for _, r := range ranges {
		if mergedCount == 0 {
			ranges[0] = r
			mergedCount = 1
			continue
		}

		last := &ranges[mergedCount-1]
		if r.start <= last.end+1 {
			if r.end > last.end {
				last.end = r.end
			}
		} else {
			ranges[mergedCount] = r
			mergedCount++
		}
	}
	mergedRanges := ranges[:mergedCount]

	// Count ingredients in any merged range with a linear two-pointer scan.
	count := 0
	rangeIdx := 0
	for _, ingredient := range ingredients {
		for rangeIdx < len(mergedRanges) && ingredient > mergedRanges[rangeIdx].end {
			rangeIdx++
		}
		if rangeIdx == len(mergedRanges) {
			break
		}
		if ingredient >= mergedRanges[rangeIdx].start {
			count++
		}
	}

	fmt.Printf("Part 1: %d\n", count)

	var total uint64 = 0
	for _, r := range mergedRanges {
		total += r.end - r.start + 1
	}

	fmt.Printf("Part 2: %d\n", total)
}
