mkdir -p build/

g++ -o build/program *.h *.cpp -std=c++23 -O2 -Wall -Wextra -Werror -lssl -lcrypto