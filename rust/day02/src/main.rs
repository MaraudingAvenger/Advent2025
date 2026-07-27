use common::read_lines;

fn is_repeated_pattern(n: u64) -> bool {
    let n_digits = (n as f64).log10() as u64 + 1;

    for pattern_length in 1..=(n_digits/2) {
        if n_digits % pattern_length != 0 {
            continue;
        }

        let divisor = 10u64.pow(pattern_length as u32);
        let pattern = n % divisor;
        
        let mut matches = true;
        let mut temp: u64 = n;

        while temp > 0 {
            if temp % divisor != pattern {
                matches = false;
                break;
            }
            temp /= divisor;
        }
        if matches {
            return true;
        }
    }
    false
}

fn main() {
    let dirty_lines = read_lines("C:\\Users\\marau\\dev\\advent\\Advent2025\\inputs\\day2.txt");
    let mut invalid_value: u64 = 0;
    
    for dirty_line in &dirty_lines {
        for range in dirty_line.trim().split(','){    
            let line = range.trim();
            let low: u64 = line[0..line.find('-').expect("Could not find range")].parse().expect("Could not parse low");
            let high: u64 = line[line.find('-').expect("Could not find range") + 1..].parse().expect("Could not parse high");

            for i in low..=high {
                let n_digits = (i as f64).log10() as u64 + 1;
                if n_digits % 2 == 1 { continue; }
                let first_half = i / 10u64.pow(n_digits as u32 / 2);
                let second_half = i % 10u64.pow(n_digits as u32 / 2);
                if first_half == second_half {
                    invalid_value += i;
                }
            }
        }
    }
    println!("Part 1: {}", invalid_value);

    invalid_value = 0;
    for dirty_line in &dirty_lines {
        for range in dirty_line.trim().split(','){    
            let line = range.trim();
            let low: u64 = line[0..line.find('-').expect("Could not find range")].parse().expect("Could not parse low");
            let high: u64 = line[line.find('-').expect("Could not find range") + 1..].parse().expect("Could not parse high");

            for i in low..=high {
                if is_repeated_pattern(i) {
                    invalid_value += i;
                }
            }
        }
    }
    println!("Part 2: {}", invalid_value);
}
