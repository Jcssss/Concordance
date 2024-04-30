#!/usr/bin/env python3

import sys

# indexes for indexed line tuples
TEXT_BEFORE = 0
INDEXED_WORD = 1
TEXT_AFTER = 2

ignore_words = []
lines_to_index = []
indexed_lines = []

# Reads in lines to index from stdin
def read_input ():

    reading_ignore_words = True

    # skips the first 2 lines, and reads the rest
    lines = sys.stdin.readlines()

    # tests that input is version 1
    if lines[0].strip() == "1":
        print("Input is version 1, concord2.py expected version 2")
        exit()
    
    # for each line
    for line in lines[2:]:

        # if the current line is an ignore word
        if reading_ignore_words:

            # checks if should switch to indexed lines
            if line.strip() == '""""':
                reading_ignore_words = False

            # if the line isn't empty
            elif not line.strip() == "":
                ignore_words.append(line.strip())
        
        # checks that line isn't empty
        elif not line.strip() == "":
            lines_to_index.append(line.strip())


# Creates tuples storing the indexed lines
def create_indexed_line_tuples ():

    # for each line to index
    for line in lines_to_index:

        # splits the current line into individual words
        split_line = line.split(" ")

        # counts the current word being checked in the list
        cur_index = 0

        # for each word in the line
        for cur_word in split_line:
            is_match = False

            # for each word in the ignore words
            for word in ignore_words:

                # compares the words in lowercase
                if cur_word.lower() == word.lower():
                    is_match = True
                    break
            
            # if the word is not a word to ignore
            if not is_match:

                # creates a new tuple, storing words before and after indexed word, and the indexed word
                new_indexed_tuple = (split_line[:cur_index], cur_word.upper(), split_line[cur_index + 1:])
                indexed_lines.append(new_indexed_tuple)

            cur_index += 1

# sorts the indexed lines
def sort_indexed_lines ():

    indexed_lines.sort(key=lambda e: e[INDEXED_WORD])

# prints the indexed line tuples
def print_output ():

    # for each indexed line
    for line in indexed_lines:
        
        # tracks the number of spots and words to output
        before_count = 29
        words_back = 0

        # for each word before the indexed word, counting backwards
        for i in range (len(line[TEXT_BEFORE]) - 1, -1, -1):

            # gets current word
            word = line[TEXT_BEFORE][i]

            # if printing the word would pass position 10
            if before_count - len(word) < 10:
                break
            
            # increments variables
            before_count -= len(word) + 1
            words_back += 1

        # tracks the number of spots left after the indexed word
        after_count = 60 - len(line[INDEXED_WORD])
        words_forward = 0

        # for each word after the indexed word
        for word in line[TEXT_AFTER]:

            # if printing the word
            if after_count - len(word) < 30:
                break
            
            # increments variables
            after_count -= len(word) + 1
            words_forward += 1

        # prints the line, formatted appropriately
        words_to_output = line[TEXT_BEFORE][-words_back:] + [line[INDEXED_WORD]] + line[TEXT_AFTER][:words_forward]
        print(" " * (before_count) + " ".join(words_to_output))

def main ():
    read_input()
    create_indexed_line_tuples()
    sort_indexed_lines()
    print_output()

if __name__ == "__main__":
    main()
