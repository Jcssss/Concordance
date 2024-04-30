#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// constants
#define MAX_LINES 100
#define MAX_LINE_LENGTH 72

#define MAX_EXCLUSION_WORDS 20
#define MAX_EXCLUSION_WORD_LENGTH 22

/*
* An indexed sentence with a capitalized word
*/
typedef struct indexed_sentence {
    char sentence[MAX_LINE_LENGTH];
    char indexed_word[MAX_LINE_LENGTH];
    int sentence_num;
} IndexedSentence;

// global variables
int num_exclusion_words = 0;
int num_lines = 0;
int num_output_lines = 0;

char lines[MAX_LINES][MAX_LINE_LENGTH];
char exclusion_words[MAX_EXCLUSION_WORDS][MAX_EXCLUSION_WORD_LENGTH];
IndexedSentence output [MAX_LINES * MAX_EXCLUSION_WORDS];

void print_output();

/*
*/
void trim_word (char * original)
{   
    char *remove;

    if (strchr(original, '\n'))
    {
        remove = strstr(original, "\n");
        *remove = '\0';
    }

    if (strchr(original, '\r'))
    {
        remove = strstr(original, "\r");
        *remove = '\0';
    }
}

/*
Reads in input from stdin file stream
*/
void get_input ()
{

    char input[MAX_LINE_LENGTH];
    int status = 1;

    /*
    Ignore the first 2 lines of the input
    */
    fgets(input, MAX_LINE_LENGTH, stdin);
    fgets(input, MAX_LINE_LENGTH, stdin);

    /*
    Iterate through remaining lines
    */
    while(fgets(input, MAX_LINE_LENGTH, stdin) != NULL)
    {   
        trim_word(input);
        /*
        Checks whether line is a word to exclude or a line for indexing, and fills the appropriate array
        */
        if (status) {
            if (strncmp(input, "\"\"\"\"", 4) == 0) 
            {
                status = 0;
            } else 
            {   
                strncpy(exclusion_words[num_exclusion_words], input, strlen(input));
                num_exclusion_words++;
            }
        } else {
            strncpy(lines[num_lines], input, strlen(input));
            num_lines++;
        }
    }
}

/*
Takes a given word and capitalizes it
*/
void capitalize_word (char *cap_word, int string_length)
{   
    int i;
    for (i = 0; i < string_length; i++)
    {  
        *cap_word = toupper(*cap_word);
        cap_word++;
    }
}

/*
Creates appropriate sentences with capitalized words
*/
void capitalize_lines ()
{
    char * t;
    char cur_line[MAX_LINE_LENGTH];
    int i;
    int j;
    char temp[MAX_LINE_LENGTH];

    int num_indexed_words;
    int num_word_in_line;
    int string_length;
    int is_exclude;

    /*
    Iterates through every line that must be indexed
    */
    for (i = 0; i < num_lines; i++)
    {   
        num_indexed_words = 0;
        string_length = 0;
        is_exclude = 0;
        temp[0] = '\0';

        /*
        Copies current line and tokenizes it
        */
        strncpy(cur_line, lines[i], MAX_LINE_LENGTH);
        t = strtok(cur_line, " ");

        /*
        Iterates through each word in the given line
        */
        while (t) 
        {   
            string_length = strlen(t);
            is_exclude = 0;

            /*
            Checks that there are exclusion words
            */
            if (num_exclusion_words != 0) {
                /*
                Iterates through each exclusion word
                */
                for (j = 0; j < num_exclusion_words; j++)
                {   
                    /*
                    Checks if current word matches an exclusion word
                    */
                    if (strncmp(t, exclusion_words[j], MAX_LINE_LENGTH) == 0) 
                    {   
                        is_exclude = 1;
                        break;
                    }
                }
            }

            /*
            If the word should be indexed, we create a new indexed_sentence object and catch it up to where every other sentence copy is currently at
            */
            if (!is_exclude) 
            {
                num_indexed_words++;
                strncat(output[num_output_lines + num_indexed_words - 1].sentence, temp, MAX_LINE_LENGTH);

                output[num_output_lines + num_indexed_words - 1].sentence_num = num_output_lines + num_indexed_words;
            }

            /*
            Updates the temp variable, which is used to catch new sentence copies up to pre-existing sentences
            */
            strncat(temp, t, string_length);
            strncat(temp, " ", 2);

            char cur_copy[string_length];
            strncpy(cur_copy, t, string_length);

            t = strtok(NULL, " ");

            /*
            Iterates through each copy of the given line. One copy per word to be indexed. One by one, we append each word in the line to the end of the line's copies.
            */
            for (j = 0; j < num_indexed_words; j++) 
            {
                /*
                Checks if the current word should be capitalized in this indexed line
                */
                if (!is_exclude && j == num_indexed_words - 1) 
                {   
                    char cap_word[string_length];

                    /*
                    Copies the current word and capitalizes it
                    */
                    strncpy(cap_word, cur_copy, string_length);
                    capitalize_word(cap_word, string_length);

                    /*
                    Appends the capitalized word to the end of the sentence
                    */
                    strncat(output[num_output_lines + j].sentence, cap_word, string_length);

                    /*
                    Stores the indexed word in the current indexed_sentence object
                    */
                    strncpy(output[num_output_lines + j].indexed_word, cap_word, string_length);

                } else {

                    /*
                    Appends the non-capitalized word to the end of the sentence
                    */
                    strncat(output[num_output_lines + j].sentence, cur_copy, string_length);
                }

                /*
                Appends a space to the end of the sentence
                */
                if (t) {
                    strncat(output[num_output_lines + j].sentence, " ", 2);
                }
            }
        }

        num_output_lines += num_indexed_words;
    }
}

/*
Swaps two elements in an array of IndexedSentences
*/
void swap (IndexedSentence * one, IndexedSentence * two) {
    IndexedSentence temp = *one;
    *one = *two;
    *two = temp; 
}

/*
Uses a recursive quick sort algorithm to sort the contents of an array
*/
void quick_sort (IndexedSentence * unsorted_sentences, int length) {

    IndexedSentence *pivot = &unsorted_sentences[length - 1];
    int i = -1;
    int j;

    /*
    Iterates through each element in the array, except the last
    */
    for (j = 0; j < length - 1; j++) {
        IndexedSentence *cur = &unsorted_sentences[j];

        /*
        If the current element is less than the pivot element, swaps the current element to the appropriate position
        */
        if (strncmp(pivot->indexed_word, cur->indexed_word, MAX_LINE_LENGTH) > 0) {
            i++;
            swap(&unsorted_sentences[i], cur);
        } else if (strncmp(pivot->indexed_word, cur->indexed_word, MAX_LINE_LENGTH) == 0) {
            
            if (pivot->sentence_num > cur->sentence_num) {
                i++;
                swap(&unsorted_sentences[i], cur);
            }
        }    
    }

    /*
    Moves the pivot element to the appropriate position
    */
    i++;
    swap(&unsorted_sentences[i], pivot);

    /*
    Recursively calls itself on the upper and lower halves of the array
    */
    if (i < length && length - 1 - i > 1) {
        quick_sort(&unsorted_sentences[i + 1], length - 1 - i);
    }

    if (i > 1) {
        quick_sort(&unsorted_sentences[0], i);
    }
}

/*
Prints the indexed sentences
*/
void print_output () {
    int i;

    for (i=0; i < num_output_lines; i++) 
    {
        printf("%s\n", output[i].sentence);
    }
}

int main(int argc, char *argv[]) 
{
    get_input();
    capitalize_lines();
    quick_sort(output, num_output_lines);
    print_output();
}
