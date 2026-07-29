# Advent of Code 2025

This year is only 12 days so I'm going to finish this one... even though I'm hitting it starting in January...

## Current Folder Structure

```text
.
├── README.md
├── cpp
│   ├── Makefile
│   ├── run.sh
│   ├── build
│   ├── common
│   │   └── utils.h
│   ├── day01
│   │   ...
│   └── day10
├── go
│   ├── go.mod
│   ├── common
│   │   └── utils.go
│   ├── day01
│   │   ...
│   └── day10
├── inputs
│   ├── day1.txt
│   │   ...
│   └── day10.txt
├── python
└── rust
    ├── Cargo.toml
    ├── common
    │   └── src
    │       └── utils.rs
    ├── day01
    │   └── Cargo.toml
    │   └── src
    │       └── main.rs
    │   ...
    └── day10
```

## Cpp

I'm using C++23 for everything in C++ land. That doesn't mean I'm necessarily using C++23 paradigms, just that I'm making all the good libs available for use.

The C++ workflow is centralized now:
- One shared build entrypoint at cpp/Makefile
- One shared utility header at cpp/common/utils.h
- One shared runner at cpp/run.sh

### Make Commands

Run all commands from the cpp directory.

Build every day folder that matches day*:

```bash
make all
```

Build exactly one folder by name:

```bash
make day01
```

Strict target behavior:
- make day01 builds the folder named day01 and writes binary build/day01
- make day1 looks for a folder literally named day1
- If the folder does not exist, make fails

Create the build directory only:

```bash
make build
```

Remove all compiled binaries:

```bash
make clean
```

Run all built day executables sequentially:

```bash
./run.sh
```

## Rust

This is just so I don't lose everything I've learned in Rust. I'm not great at the language and just trying to develop/not lose all my rust knowledge.

### Cargo Commands

Run each day with:

```bash
cargo run -p day01
```

To create a new day folder, run:

```bash
cargo new day03 --vcs none
```

## Go

I'm also keeping a Go version around so I can practice it without letting it completely atrophy.

Run it with:

```bash
go run ./day01
```

## Python

Python is the fallback when I want to move quickly or keep the solution simple.

