package main

import (
	"fmt"
	"strings"

	utils "github.com/MaraudingAvenger/Advent2025/common"
)

func main() {
	lines, err := utils.ReadInputFile(utils.FindInputFile("inputs/day4.txt"))
	if err != nil {
		fmt.Println("Error reading input file:", err)
		return
	}
	var linesList [][]rune
	for _, line := range strings.Fields(lines) {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}
		linesList = append(linesList, []rune(line))
	}

	accessible := 0

	for i := 0; i < len(linesList); i++ {
		for j := 0; j < len(linesList[i]); j++ {
			if linesList[i][j] == '@' {
				// Count the number of '@' symbols in the surrounding cells
				atCount := 0
				for di := -1; di <= 1; di++ {
					for dj := -1; dj <= 1; dj++ {
						if di == 0 && dj == 0 {
							continue
						}
						ni, nj := i+di, j+dj
						if ni >= 0 && ni < len(linesList) && nj >= 0 && nj < len(linesList[ni]) {
							if linesList[ni][nj] == '@' {
								atCount++
							}
						}
					}
				}

				if atCount < 4 {
					accessible++
				}
			}
		}
	}
	fmt.Printf("Part 1: %d\n", accessible)

	// Part 2: Remove the '@' symbols that are accessible and loop again until no more can be removed
	accessible = 0

	removed := false
	for {
		for i := 0; i < len(linesList); i++ {
			for j := 0; j < len(linesList[i]); j++ {
				if linesList[i][j] == '@' {
					// Count the number of '@' symbols in the surrounding cells
					atCount := 0
					for di := -1; di <= 1; di++ {
						for dj := -1; dj <= 1; dj++ {
							if di == 0 && dj == 0 {
								continue
							}
							ni, nj := i+di, j+dj
							if ni >= 0 && ni < len(linesList) && nj >= 0 && nj < len(linesList[ni]) {
								if linesList[ni][nj] == '@' {
									atCount++
								}
							}
						}
					}

					if atCount < 4 {
						linesList[i][j] = '.'
						accessible++
						removed = true
					}
				}
			}
		}
		if !removed {
			break
		}
		removed = false
	}
	fmt.Printf("Part 2: %d\n", accessible)
}
