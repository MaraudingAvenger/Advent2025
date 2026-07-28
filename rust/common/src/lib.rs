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

pub fn find_input_file(filename: &str) -> String {
    let path = std::path::Path::new(filename);
    if path.exists() {
        return filename.to_string();
    }

    let mut current_dir = std::env::current_dir().expect("Could not get current directory");
    loop {
        let input_path = current_dir.join(filename);
        if input_path.exists() {
            return input_path.to_str().unwrap().to_string();
        }
        if !current_dir.pop() {
            break;
        }
    }

    panic!("Could not find input file: {}", filename);
}
