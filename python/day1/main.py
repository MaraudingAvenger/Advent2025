def main(fn: str = 'input.txt') -> None:
    with open(fn, 'r') as file:
        zeroes = 0
        dial = 50
        size = 100
        for line in file:
            direction = line[0]
            amount = int(line[1:].strip())
            match direction:
                case 'R':
                    dial = (dial + amount) % size
                case 'L':
                    dial = (dial + size - (amount % size)) % size
            if dial == 0:
                zeroes += 1
        print("Part 1:", zeroes)

    with open(fn, 'r') as file:
        dial = 50
        zeroes = 0
        size = 100
        for line in file:
            direction = line[0]
            amount = int(line[1:].strip())
            match direction:
                case 'R':
                    for _ in range(amount):
                        dial = (dial + 1) % size
                        if dial == 0:
                            zeroes += 1
                case 'L':
                    for _ in range(amount):
                        dial = (dial + size - 1) % size
                        if dial == 0:
                            zeroes += 1
        print("Part 2:", zeroes)

if __name__ == '__main__':
    main()