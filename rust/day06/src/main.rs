use common::read_lines;
use common::find_input_file;

fn main() {
    let lines = read_lines(&find_input_file("inputs/day6.txt"));

    // file looks like:
    //   1 123  23
    //  21 312 132
    // 321  12   1
    //   +   *   *

    // numbers in the file are arranged in columns, separated by whitespace.
    // the last line contains the operations to perform on each column of numbers.

    let mut columns: Vec<Vec<u64>> = Vec::new();
    for i in 0..lines.len() - 1 {
        let line = &lines[i];
        let numbers: Vec<u64> = line
            .split_whitespace()
            .map(|s| s.parse::<u64>().unwrap())
            .collect();
        for (j, num) in numbers.iter().enumerate() {
            if columns.len() <= j {
                columns.push(Vec::new());
            }
            columns[j].push(*num);
        }
    }

    let mut result: u64 = 0;
    for (i, operand) in lines[lines.len() - 1].split_whitespace().enumerate() {
        result += match operand {
            "+" => columns[i].iter().sum::<u64>(),
            "*" => columns[i].iter().product::<u64>(),
            _ => panic!("Unknown operand: {}", operand),
        };
    }

    println!("Part 1: {}", result);

    // Part 2: parse numbers pivoted 90 degrees vertically
    result = 0;
    let mut numbers: Vec<String> = Vec::new();
    for i in (0..lines[0].len()).rev() {
        let mut num_str = String::new();
        for j in 0..lines.len()-1 {
            let c = lines[j].chars().nth(i).unwrap();
            if c == ' ' {
                continue;
            }
            num_str.push(c);
        }
        if num_str.trim().len() > 0 {
            numbers.push(num_str);
        }
        match lines[lines.len()-1].chars().nth(i).unwrap() {
            '+' => {
                let sum: u64 = numbers.iter().map(|s| s.trim().parse::<u64>().unwrap()).sum();
                result += sum;
                numbers.clear();
            },
            '*' => {
                let product: u64 = numbers.iter().map(|s| s.trim().parse::<u64>().unwrap()).product();
                result += product;
                numbers.clear();
            },
            _ => {}
        }
    }

    println!("Part 2: {}", result);

}
