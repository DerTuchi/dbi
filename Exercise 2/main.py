import math
import os
import random
from datetime import datetime
import numpy
import psycopg2
from time import time

#---------------------------------------- Exersice 1 -----------------------------------------

def count_bits(value_list):
    def bit_length(value):
        if value >= 0:
            return len("{0:b}".format(value))
        else:
            return 32
    return sum(bit_length(value) for value in value_list)


def for_encoding(value_list, frame_size):
    result = []
    # Split into Chunks with Frame_size
    chunks = [value_list[x: x + frame_size] for x in range(0, len(value_list), frame_size)]
    for chunk in chunks:
        # find FOR-Element with min
        ref = min(chunk)
        # Subtract
        for temp in range(len(chunk)):
            if temp.__eq__(chunk.index(ref)):
                pass
            else:
                chunk[temp] = chunk[temp] - ref
        # Put chunks together
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
    dictionary = {}
    counter = 0
    for i in range(len(result)):
        if not result[i] in dictionary.keys():
            dictionary.update({result[i]: counter})
            counter += 1
        result[i] = dictionary.get(result[i])
    return result


def test(value_list):
    print(f'Needed Bits:\n'
          f'\tSource\t\t\t\t: {count_bits(value_list)}\n'
          f'\tFOR (Frame-Size=2)\t: {count_bits(for_encoding(value_list, 2))}\n'
          f'\tDELTA\t\t\t\t: {count_bits(delta_encoding(value_list))}\n'
          f'\tDICT\t\t\t\t: {count_bits(dictionary_encoding(value_list))}\n')


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

#---------------------------------------- Exersice 3 -----------------------------------------
path_sql = 'D:/Code/Private Code/dbi/Exercise 1/queries/'


# Connect to psql db
conn = psycopg2.connect(host="localhost", database="dbi", user="postgres", password="123")
cur = conn.cursor()

conn_1 = psycopg2.connect(host="localhost", database="dbi_compressed", user="postgres", password="123")
cur_compressed = conn_1.cursor()

for file in os.listdir(path_sql):
    # Open sql file and read content of it
    fd = open(path_sql + file, 'r')
    query = fd.read()
    fd.close()

    try:
        # Apply query from sql file on psql db with the cursor
        start_time = time()
        cur.execute(query)
        end_time = time()
        print(f'Execution time - {file}: {end_time - start_time} Seconds.')

        start_time = time()
        cur_compressed.execute(query)
        end_time = time()
        print(f'Execution time Compressed - {file}: {end_time - start_time} Seconds.\n')
    except ValueError:
        print(f'{file} could not execute')