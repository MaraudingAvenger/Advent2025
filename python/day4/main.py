def main(fn: str = "input.txt") -> None:
    table = []
    with open(fn, 'r') as file:
        for line in file.readlines():
            table.append(line.strip().split())
    accessible = 0
    delta = [-1, 0, 1]
    for i in range(len(table)):
        for j in range(len(table[i])):
            if table[i][j] == '.':
                continue
            
            count = 0
            for di in delta:
                for dj in delta:
                    if di == dj == 0:
                        continue
                    x, y = i + di, j + dj
                    if 0 <= x < len(table)-1 and 0 <= y < len(table[i])-1:
                        if table[x][y] == '@':
                            count += 1
            if count < 4:
                accessible += 1
            break
        break
    print("Part 1:", accessible)


if __name__ == '__main__':
    main()