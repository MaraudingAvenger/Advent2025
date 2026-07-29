CXX := g++
CXXFLAGS := -std=c++23 -O2 -Wall -Wextra -Werror
LDFLAGS := -lssl -lcrypto # this is not actually used anywhere

DAY_DIRS := $(sort $(shell find . -mindepth 1 -maxdepth 1 -type d -name 'day*' -printf '%f\n'))

.PHONY: all clean build $(DAY_DIRS) day%

all: $(DAY_DIRS)

build:
	@mkdir -p build

# Build discovered day* folders for `make all`.
$(DAY_DIRS): %: build
	@if [ ! -d "$@" ]; then \
		echo "Error: folder '$@' does not exist."; \
		exit 1; \
	fi
	@srcs=$$(find "$@" -maxdepth 1 -type f -name '*.cpp' | sort); \
	if [ -z "$$srcs" ]; then \
		echo "Error: no .cpp files found in '$@'."; \
		exit 1; \
	fi; \
	echo "Building $@ -> build/$@"; \
	$(CXX) $(CXXFLAGS) -o "build/$@" $$srcs $(LDFLAGS)

# Build any explicit day target exactly as named (e.g. make day01 or make day1).
day%: build
	@if [ ! -d "$@" ]; then \
		echo "Error: folder '$@' does not exist."; \
		exit 1; \
	fi
	@srcs=$$(find "$@" -maxdepth 1 -type f -name '*.cpp' | sort); \
	if [ -z "$$srcs" ]; then \
		echo "Error: no .cpp files found in '$@'."; \
		exit 1; \
	fi; \
	echo "Building $@ -> build/$@"; \
	$(CXX) $(CXXFLAGS) -o "build/$@" $$srcs $(LDFLAGS)

clean:
	rm -rf build
