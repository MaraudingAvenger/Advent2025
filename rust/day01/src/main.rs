use common::read_lines;
use common::find_input_file;

fn main() {
    let lines = read_lines(&find_input_file("inputs/day1.txt"));
    let mut dial = 50;
    let mut zero_count = 0;
    const DIAL_SIZE: i32 = 100;

    // Part 1
    for line in &lines {
        let direction = &line[0..1];
        let value: i32 = line[1..].parse().expect("Could not parse value");
        match direction {
            "L" => dial = (dial + DIAL_SIZE - value) % DIAL_SIZE,
            "R" => dial = (dial + value) % DIAL_SIZE,
            _ => panic!("Unknown direction"),
        }
        if dial == 0 {
            zero_count += 1;
        }
    }
    println!("Part 1: {}", zero_count);

    // Part 2
    dial = 50;
    zero_count = 0;
    for line in &lines {
        let direction = &line[0..1];
        let value: i32 = line[1..].parse().expect("Could not parse value");
        match direction {
            "L" => {
                for _ in 0..value {
                    dial -= 1;
                    if dial == 0 {
                        zero_count += 1;
                    }
                    if dial < 0 {
                        dial = DIAL_SIZE - 1;
                    }
                }
            }
            "R" => {
                for _ in 0..value {
                    dial += 1;
                    if dial >= DIAL_SIZE {
                        dial = 0;
                    }
                    if dial == 0 {
                        zero_count += 1;
                    }
                }
            }
            _ => panic!("Unknown direction"),
        }
    }
    println!("Part 2: {}", zero_count);
}
