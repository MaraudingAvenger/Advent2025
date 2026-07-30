use common::read_lines;
use common::find_input_file;

fn main() {
    let lines = read_lines(&find_input_file("inputs/day5.txt"));
    let mut ranges = Vec::new();
    let mut ingredients = Vec::new();

    let mut parsing_ranges = true;
    for line in lines {
        if line.is_empty() {
            parsing_ranges = false;
            continue;
        }

        if parsing_ranges {
            let parts = line
                .trim()
                .split("-")
                .map(|s| s.parse::<usize>().expect("Could not parse range"))
                .collect::<Vec<usize>>();
            ranges.push((parts[0], parts[1]));
        }
        else {
            ingredients.push(line.trim().parse::<usize>().expect("Could not parse ingredient"));
        }
    }

    // combine overlapping ranges
    ranges.sort_by(|a, b| a.0.cmp(&b.0));
    let mut combined_ranges = Vec::new();
    let mut current_range = ranges[0];
    for &range in &ranges[1..] {
        if range.0 <= current_range.1 {
            current_range.1 = current_range.1.max(range.1);
        } else {
            combined_ranges.push(current_range);
            current_range = range;
        }
    }
    combined_ranges.push(current_range);
    ranges = combined_ranges;
    ingredients.sort();

    let mut count = 0;
    for ingredient in ingredients {
        for &(start, end) in &ranges {
            if ingredient >= start && ingredient <= end {
                count += 1;
                break;
            }
        }
    }
    println!("Part 1: {}", count);

    let mut total_fresh: usize = 0;
    for &(start, end) in &ranges {
        total_fresh += end - start + 1;
    }

    println!("Part 2: {}", total_fresh);
}
