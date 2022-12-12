import math
import random
from datetime import datetime
import numpy
from enum import Enum

#---------------------------------------- Exersice 1 -----------------------------------------
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

#---------------------------------------- Exersice 2 -----------------------------------------

class CacheEntry:
    def __init__(self, key, value):
        self.__key = key
        self.__value = value

    def __eq__(self, other):
        return self.__key == other.__key

    def __str__(self):
        return f'({self.__key}: {self.__value})'

    @property
    def key(self):
        return self.__key

    @property
    def value(self):
        return self.__value

# Used to Save Time information
class TimeBuffer:
    def __init__(self, k):
        self.__buffer = {}
        self.__k = k

    def put(self, key):
        if not self.__buffer.__contains__(key):
            temp = []
            for i in range(self.__k):
                temp.append(numpy.inf)
            self.__buffer [key] = temp

        temp = self.__buffer.get(key) [1:]
        now = datetime.now()
        temp.append(int(now.strftime("%Y%m%d%H%M%S")))
        self.__buffer [key] = temp

    def get(self):
        return self.__buffer

class Cache:
    def __init__(self, capacity, alg_type, k=1):
        if not alg_type in ["LRUK", "LIFO", "FIFO"]:
            raise ValueError ("Parameter alg_type only allows: 'LRUK', 'LIFO', 'FIFO' as Values")
        self.__data = []
        self.alg_type = alg_type
        if alg_type == "LRUK":
            self.__buffer = TimeBuffer(k)
        self.__capacity = capacity
        self.__element_count = 0

    def __str__(self):
        return f"[{', '.join(([str(entry) for entry in self.__data]))}"

    def put(self, key, value):
        if self.__element_count < self.__capacity:
            cache_entry = CacheEntry(key, value)
            self.__data.append(cache_entry)
            if self.alg_type == "LRUK":
                self.__buffer.put(key)
            self.__element_count += 1
        else:
            if self.alg_type == "LRUK":
                buffer = self.__buffer.get()
                loser = None
                for i in buffer.keys():
                    if loser is None:
                        loser = i
                    else:
                        temp0 = buffer.get(loser)
                        temp1 = buffer.get(i)
                        delta_temp0 = temp0[-1] - temp0[0]
                        delta_temp1 = temp1[-1] - temp1[0]
                        if not math.isinf(temp0[0]) and (math.isinf(temp1[0]) or delta_temp0 < delta_temp1):
                            loser = i
                for temp in self.__data:
                    if temp.key == loser:
                        self.__data.pop(self.__data.index(temp))
            elif self.alg_type == "LIFO":
                self.__data.pop(-1)
            elif self.alg_type == "FIFO":
                self.__data.pop(0)
            self.__data.append(CacheEntry(key, value))

    def get(self, key):
        for idx in range(self.__element_count):
            if self.__data[idx].key == key:
                #Added Key to the TimeBuffer
                if self.alg_type == "LRUK":
                    self.__buffer.put(key)

                self.__data.append(self.__data.pop(idx))
                return self.__data[-1]
        return None



cache_capacity = 8
cache = Cache(cache_capacity, "FIFO")
for i in range(cache_capacity):
    cache.put(i, i+1)
str(cache)
cache.put(cache_capacity, cache_capacity + 1)
str(cache)
str(cache.get(1))
str(cache)
