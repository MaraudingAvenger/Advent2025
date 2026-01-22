import math

def main(fn: str = 'input.txt') -> None:
    with open(fn, 'r') as file:
        invalid_numbers = 0
        for [low,high] in [l.split('-') for l in file.read().strip().split(',')]:
            for n in range(int(low), int(high)+1):
                if (int(math.log10(n))+1) % 2 == 1:
                    continue
                str_n = str(n)
                if str_n[0:len(str_n)//2] == str_n[len(str_n)//2:]:
                    invalid_numbers += n
        print("Part 1:", invalid_numbers)

    with open(fn, 'r') as file:
        invalid_numbers = 0
        for [low,high] in [l.split('-') for l in file.read().strip().split(',')]:
            for n in range(int(low), int(high)+1):
                if (int(math.log10(n))+1) < 2:
                    continue
                str_n = str(n)
                for i in range(1, len(str_n)//2 + 1):
                    # check if the string is made up of i length repeating patterns
                    if len(str_n) % i == 0 and str_n[0:i] * (len(str_n)//i) == str_n:
                        invalid_numbers += n
                        break
        print("Part 2:", invalid_numbers)

if __name__ == '__main__':
    main()