import sys
import re
from collections import namedtuple
from typing import List

IndexedLine = namedtuple("IndexedLine", 
                        ["before", "after", "cap_word"])

class concord:

    def __init__(self, input=None, output=None):

        self.__input = input
        self.__output = output
        self.__index_lines()

    '''
    Performs the full concordance process
    '''
    def __index_lines (self):

        input_tuple = self.__read_input()
        ignore_words = input_tuple[0]
        lines_to_index = input_tuple[1]

        indexed_line_tuples = (
            self.__create_indexed_line_tuples(ignore_words, lines_to_index))

        self.__sort_indexed_lines(indexed_line_tuples)
        self.__string_output = self.__format_output(indexed_line_tuples)

        if self.__output != None:
            self.__write_output()

    '''
    Reads input from given file or stdin if no file given
    '''
    def __read_input(self):

        if self.__input == None:
            lines = sys.stdin.readlines()
        else:
            file_contents = open(self.__input, "r")
            lines = re.split("\n", file_contents.read())
            file_contents.close()

        # tests that input is version 2
        if lines[0].strip() != "2":
            print("Input is version 1, concord2.py expected version 2")
            exit()
        
        # skips the first two lines and reads ignore words
        ignore_words = ""
        i = 2
        while (lines[i].strip() != '""""'):
            ignore_words += lines[i].strip() + " "
            i += 1

        # removes the last extra space
        ignore_words.rstrip()

        # reads the lines to index
        lines_to_index = [
            line.strip() for line in lines[i + 1:] if line.strip() != ""]

        return(ignore_words, lines_to_index)

    '''
    Indexes the given lines to index
    '''
    def __create_indexed_line_tuples(self, 
                                     ignore_words: str, 
                                     lines_to_index: List[str]
                                     ) -> List[IndexedLine]:

        indexed_line_tuples = []

        # for each line, generates all necessary indexed lines for output
        for line in lines_to_index:
            new_indexed_lines = self.__index_single_line(line, ignore_words)
            indexed_line_tuples += new_indexed_lines

        return indexed_line_tuples

    '''
    Given a single line, generates all necessary indexed versions of the line
    '''
    def __index_single_line (self, 
                             line: str, 
                             ignore_words: str
                             ) -> List[IndexedLine]:

        words_in_line = line.split(" ")

        # finds which words in the line must be indexed
        words_to_index = [
            word 
            for word in words_in_line
            if not re.search(r"\b" + word + r"\b", 
                             ignore_words, re.IGNORECASE)
        ]

        new_indexed_line_tuples = []

        for word in words_to_index:

            # finds the words before and after indexed word
            before_after = re.split(r"\b" + word + r"\b", line) 

            # creates and adds new IndexedLine
            new_indexed = IndexedLine(before_after[0], 
                                      before_after[1], 
                                      word.upper())
            new_indexed_line_tuples.append(new_indexed)

        return new_indexed_line_tuples

    '''
    Sorts a list of indexed lines
    '''
    def __sort_indexed_lines (self, indexed_lines: List[IndexedLine]):
        indexed_lines.sort(key=lambda e: e[2])

    '''
    Formats and creates the output
    '''
    def __format_output (self, indexed_lines: List[IndexedLine]) -> List[str]:
        tot_char_before = 21
        tot_char_after = 32
        before, after, cap_word = [""] * 3
        string_output = []

        for line_tuple in indexed_lines:
            cap_word = line_tuple.cap_word

            # checks whether the before output is larger than needed
            if len(line_tuple.before) >= tot_char_before:
                before = line_tuple.before[-tot_char_before:]
                before = self.__format_front(before)
            else:
                before = line_tuple.before

            # checks whether the after output is larger than needed
            length = tot_char_after - len(cap_word)
            if len(line_tuple.after) >= length:
                after = self.__format_back(line_tuple.after[0:length])
            else:
                after = line_tuple.after

            # creates final indexed line
            new_output = before + cap_word + after
            new_output = new_output.rstrip()
            new_output = " " * (tot_char_before - len(before) + 8) + new_output

            string_output.append(new_output)

        return string_output

    '''
    Formats the output before the indexed word
    '''
    def __format_front(self, to_format: str) -> str:

        formatted_string = ""

        # starting from the first character searches for a space moving
        # forwards
        for i in range(0, len(to_format)):
            if to_format[i] == " ":
                formatted_string = to_format[i:]
                break

        return formatted_string

    '''
    Formats the output after the indexed word
    '''
    def __format_back(self, to_format: str) -> str:

        formatted_string = ""

        # starting from the last character searches for a space moving
        # backwards
        for i in range(len(to_format) - 1, 0, -1):
            if to_format[i] == " ":
                formatted_string = to_format[:i]
                break

        return formatted_string

    '''
    Writes required output to given file
    '''
    def __write_output(self):
        file = open(self.__output, "w")
        str_output = "\n".join(self.__string_output)
        file.write(str_output + "\n")
        file.close()

    '''
    Returns the given output
    '''
    def full_concordance(self):
        return self.__string_output
