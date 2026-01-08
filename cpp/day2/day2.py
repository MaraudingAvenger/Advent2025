import math

lines = sorted([list(map(int, s.split('-')))
         for s in open('input.txt').read().strip().split(',')],
         key=lambda x: x[0])

count = 0

for [a, b] in lines:
    digits = int(math.log10(b)) + 1
    if digits % 2 == 0:
        for i in range(a, b+1):
            first_half = i // (10 ** (digits // 2))
            second_half = i % (10 ** (digits // 2))
            if first_half == second_half:
                count += i

print(count)