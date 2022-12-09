import random


def bit_length(value):
    if value >= 0:
        return len("{0:b}".format(value))
    else:
        return 32


def count_bits(value_list):
    return sum(bit_length(value) for value in value_list)


def for_encoding(value_list, frame_size):
    result = []
    # First Delta Encoding
    delta = delta_encoding(value_list)
    # Second Split into Chunks with Frame_size
    chunks = [delta[x: x + frame_size] for x in range(0, len(delta), frame_size)]
    for chunk in chunks:
        # Third Bit packing
        length = max(bit_length(temp) for temp in chunk)
        chunk.insert(0, length)
        result.extend(chunk)
    return result


def delta_encoding(value_list):
    result = []
    result.extend(value_list)
    last = 0
    for i in range(len(result)):
        current = result[i]
        result[i] = current - last
        last = current
    return result


def dictionary_encoding(value_list):
    result = []
    result.extend(value_list)
    codes = {}
    for i in range(len(result)):
        codes.update({result[i]: i})
        result[i] = i
    return result


def test(value_list):
    print(f'Needed Bits:\n'
          f'\tSource\t\t\t\t: {count_bits(value_list)}\n'
          f'\tFOR (Frame-Size=2)\t: {count_bits(for_encoding(value_list, 2))}\n'
          f'\tDELTA\t\t\t\t: {count_bits(delta_encoding(value_list))}\n'
          f'\tDICT\t\t\t\t: {count_bits(dictionary_encoding(value_list))}')


values = [1, 3, 7, 12, 13, 13, 14, 17]
shuffled_values = random.sample(values, len(values))
test(values)
