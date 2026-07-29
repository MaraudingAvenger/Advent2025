use common::read_lines;
use common::find_input_file;

fn count_at_symbols(grid: &[Vec<u8>], i: usize, j: usize) -> u8 {
    let rows = grid.len() as isize;
    let cols = grid[0].len() as isize;
    let mut at_count = 0;

    for di in -1..=1 {
        for dj in -1..=1 {
            if di == 0 && dj == 0 {
                continue;
            }
            let ni = i as isize + di;
            let nj = j as isize + dj;
            if ni >= 0 && ni < rows && nj >= 0 && nj < cols && grid[ni as usize][nj as usize] == b'@' {
                at_count += 1;
            }
        }
    }

    at_count
}

fn main() {
    let mut grid: Vec<Vec<u8>> = read_lines(&find_input_file("inputs/day4.txt"))
        .into_iter()
        .map(|line| line.into_bytes())
        .collect();

    let mut accessible = 0;
    for i in 0..grid.len() {
        for j in 0..grid[0].len() {
            if grid[i][j] == b'@' && count_at_symbols(&grid, i, j) < 4 {
                accessible += 1;
            }
        }
    }

    println!("Part 1: {}", accessible);

    accessible = 0;
    loop {
        let mut removed = false;
        for i in 0..grid.len() {
            for j in 0..grid[0].len() {
                if grid[i][j] == b'@' && count_at_symbols(&grid, i, j) < 4 {
                    grid[i][j] = b'.';
                    accessible += 1;
                    removed = true;
                }
            }
        }

        if !removed {
            break;
        }
    }

    println!("Part 2: {}", accessible);
}
