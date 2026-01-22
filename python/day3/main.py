def make_number_from_digits(digits: list[int], n_digits: int) -> int:
    max_number = 0

    last_number = 0
    last_index = 0
    digits_remaining = n_digits
    while digits_remaining > 0:
        for i in range(last_index, len(digits) - digits_remaining + 1):
            if digits[i] > last_number:
                last_number = digits[i]
                last_index = i + 1
            if last_number == 9:
                break
        max_number = max_number * 10 + last_number
        last_number = 0
        digits_remaining -= 1
    return max_number

def main() -> None:
    with open('input.txt', 'r') as file:
        voltage = 0
        for line in file.readlines():
            # make the largest number possible by combining two digits
            digits = [int(s) for s in line.strip()]
            max_number = make_number_from_digits(digits, 2)
            voltage += max_number
        print("Part 1:", voltage)

    with open('input.txt', 'r') as file:
        voltage = 0
        for line in file.readlines():
            # make the largest 12 digit number possible by combining digits
            digits = [int(s) for s in line.strip()]

            max_number = make_number_from_digits(digits, 12)
            voltage += max_number
        print("Part 2:", voltage)