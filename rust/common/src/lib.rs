use std::fs::File;
use std::io::BufReader;
use std::io::BufRead;

pub fn read_lines(filename: &str) -> Vec<String> {
    let file = File::open(filename).expect("Could not open file");
    let reader = BufReader::new(file);
    reader.lines()
        .map(|line| line.expect("Could not read line"))
        .collect()
}

