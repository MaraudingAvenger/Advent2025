use common::read_lines;
use common::find_input_file;

fn find_largest_number(numbers: &String, n_digits: usize) -> Option<usize> {
    // find the largest number with n_digits in the string of numbers
    // the number does not need to be coniguous, but the digits must be in order
    let mut to_drop = numbers.len() - n_digits;
    let mut stack: Vec<char> = Vec::new();

    for i in 0..numbers.len() {
        let c = numbers.chars().nth(i).unwrap();
        if c < '0' || c > '9' {
            continue;
        }
        while to_drop > 0 && !stack.is_empty() && *stack.last().unwrap() < c {
            stack.pop();
            to_drop -= 1;
        }
        stack.push(c);
    }

    if to_drop > 0 {
        stack.truncate(stack.len() - to_drop);
    }
    let result = stack.iter().take(n_digits).collect::<String>();
    Some(result.parse::<usize>().unwrap())
}

fn main() {
    let lines = read_lines(&find_input_file("inputs/day3.txt"));

    let mut total_joltage: usize = 0;
    for line in &lines {
        if let Some(largest_number) = find_largest_number(line, 2) {
            total_joltage += largest_number;
        }
    }
    println!("Part 1: {}", total_joltage);

    total_joltage = 0;
    for line in &lines {
        if let Some(largest_number) = find_largest_number(line, 12) {
            total_joltage += largest_number;
        }
    }
    println!("Part 2: {}", total_joltage);
}
