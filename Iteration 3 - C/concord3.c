/*
 * concord3.c
 *
 * Starter file provided to students for Assignment #3, SENG 265,
 * Fall 2022.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emalloc.h"
#include "seng265-list.h"
#include <ctype.h>

#define MAX_LINE_LENGTH 102
#define MAX_EXCLUSION_WORD_LENGTH 42

/*
 * An indexed sentence with a capitalized word
 */
typedef struct indexed_sentence {
    char * before_word;
    char * indexed_word;
    char * after_word;
} IndexedSentence;

/*
 * A dynamic array of indexed sentences
 */
typedef struct indexed_array {
    IndexedSentence ** indexed_lines;
    int curr_size;
    int max_size;
} IndexedArray;

enum { START = 1, GROW = 2 };

void trim_word (char * original);
void get_input (node_t ** ex_words_head, node_t ** lines_head);
void add_line_to_output (IndexedArray *output, IndexedSentence *line);
char * capitalize_word (char *cap_word, int string_length);
int strncmp_ignore_case (char * one, char * two);
int compare (const void * a, const void * b);
void print_format_before_word (char * before);
void print_format_after_word (char * after, int index_word_length);
void print_results (IndexedArray * results);
void free_list (node_t * head);

IndexedArray * create_indexed_lines (node_t * ex_words_head, 
                                     node_t * lines_head);

IndexedSentence *new_indexed_line (char * before, 
                                   char *cap_word, 
                                   char* after);

void index_single_line (char *curr_line, 
                        node_t * ex_words_head, 
                        IndexedArray ** results);

void index_and_add (char *curr_line, 
                    int characters_before, 
                    char * indexed_word, 
                    IndexedArray ** results);

int main(int argc, char *argv[]) {   
    node_t *ex_words_head = NULL;
    node_t *lines_head = NULL;
    IndexedArray * results = NULL;

    get_input (&ex_words_head, &lines_head);

    results = create_indexed_lines(ex_words_head, lines_head);

    qsort(results->indexed_lines, results->curr_size, sizeof(IndexedSentence*), compare);

    print_results(results);

    free_list(ex_words_head);
    free_list(lines_head);
    free(results->indexed_lines);
    free(results);
}

/*
 * Given a string, removes the \n from the word
 */
void trim_word (char * original) {   
    char *remove;

    if (strchr(original, '\n')) { 
        remove = strstr(original, "\n");
        *remove = '\0';
    }
}

/*
 * Reads in input from stdin file stream, saves input to linked
 * lists
 */
void get_input (node_t ** ex_words_head, node_t ** lines_head) {
    char input[MAX_LINE_LENGTH];
    int status = 1;
    node_t * temp_node = NULL;
    
    // Checks input version
    fgets(input, MAX_LINE_LENGTH, stdin);
    trim_word(input);

    if (strncmp(input, "2", 2) != 0) {
        printf("Input is version 1, concord3 expected version 2");
        exit(0);
    } 

    // Skips second line
    fgets(input, MAX_LINE_LENGTH, stdin);
    
    // Iterate through remaining lines
    while(fgets(input, MAX_LINE_LENGTH, stdin) != NULL) {   
        trim_word(input);
        
        // Checks whether line is a word to exclude or a line for indexing
        if (status) {
            if (strncmp(input, "\"\"\"\"", 4) == 0) {
                status = 0;
            } else {   
                temp_node = new_node(input);
                *ex_words_head = add_end(*ex_words_head, temp_node);
            }
        } else if (strncmp(input, "", 2) != 0){
            temp_node = new_node(input);
            *lines_head = add_end(*lines_head, temp_node);
        }
    }
}

/*
 * Adds an indexed sentence object to the lines to output
 * Code based off of the C slides from lecture 05
 */
void add_line_to_output (IndexedArray *output, IndexedSentence *line) {

    IndexedSentence ** temp;

    if (output->indexed_lines == NULL) {
        output->indexed_lines = 
            (IndexedSentence **) emalloc(START * sizeof(IndexedSentence*));
        output->max_size = START;
        output->curr_size = 0;
    } else if (output->curr_size >= output->max_size) {
        temp = (IndexedSentence **) realloc (output->indexed_lines,
            (GROW * output->max_size) * sizeof (IndexedSentence*));
        output->max_size = GROW * output->max_size;
        output->indexed_lines = temp;
    }
    output->indexed_lines[output->curr_size] = line;
    output->curr_size++;
}

/*
Takes a given word and capitalizes it
*/
char * capitalize_word (char *cap_word, int string_length) {   
    int i;
    for (i = 0; i < string_length; i++) {  
        cap_word[i] = toupper(cap_word[i]);
    }

    return cap_word;
}

/*
 * Creates a new indexed sentence object
 */
IndexedSentence *new_indexed_line (char * before, 
                                   char *cap_word, 
                                   char* after)
{
    IndexedSentence *temp;
    temp = (IndexedSentence *)emalloc(sizeof(IndexedSentence));

    temp->before_word = before;
    temp->indexed_word = capitalize_word(cap_word, strlen(cap_word));
    temp->after_word = after;

    return temp;
}

/*
 * Compares two words ignoring case
 */ 
int strncmp_ignore_case (char * one, char * two) {
    char *temp_one = strdup(one);
    capitalize_word(temp_one, strlen(temp_one));

    char *temp_two = strdup(two);
    capitalize_word(temp_two, strlen(temp_two));

    int answer = strncmp(temp_one, temp_two, strlen(temp_one));
    
    free(temp_one);
    free(temp_two);

    return answer;
}

/*
 * Creates a new indexed sentence from the given line with
 * a given capitalized word. Adds indexed line to output.
 */
void index_and_add (char *curr_line, 
                    int characters_before, 
                    char * indexed_word, 
                    IndexedArray ** results) 
{
    IndexedSentence * new_sentence = NULL;
    char * words_before = NULL;
    char * words_after = NULL;
    char * cap_word = NULL;

    words_before = 
        (char *)emalloc((characters_before + 1) * sizeof(char));
    strncpy(words_before, curr_line, characters_before);
    words_before[characters_before] = '\0';

    cap_word = strdup(indexed_word);
    words_after = strdup(&curr_line[characters_before + strlen(indexed_word)]);

    new_sentence = 
        new_indexed_line (words_before, cap_word, words_after);                
    add_line_to_output(*results, new_sentence);
}

/*
 * Indexes a single line
 */ 
void index_single_line (char *curr_line, 
                        node_t * ex_words_head, 
                        IndexedArray ** results)
{
    char *temp, *t;
    int characters_in = 0;
    node_t * curr_node = NULL;
    
    temp = strdup(curr_line);
    t = strtok(temp, " ");

    // For every word in the line
    while (t) {  
        curr_node = ex_words_head;

        // If there are no exclusion words enters while loop
        // Otherwise, iterates through every exclusion word
        while (!ex_words_head || curr_node) {

            // checks that there are exclusion words and checks whether 
            // the current word matches the curren exclusion word
            if (ex_words_head &&
                strncmp_ignore_case(t, curr_node->text) == 0) 
            { 
                break; 

            // otherwise, if no exclusion words left or in general
            } else if (!ex_words_head || curr_node->next == NULL) {
                
                // creates an indexed line with the current word capitalized
                index_and_add (curr_line, characters_in, t, results);
                break;
            }
            curr_node = curr_node->next;
        }

        // tracks how many characters are before the current word in the line
        characters_in += strlen(t) + 1;
        t = strtok(NULL, " ");
    }

    free(temp);
}

/*
 * Creates an indexed line struct
 */
IndexedArray * create_indexed_lines (node_t * ex_words_head, 
                                     node_t * lines_head) 
{
    node_t * temp_line_node = lines_head;
    IndexedArray * results = (IndexedArray *) emalloc(sizeof(IndexedArray));

    results -> indexed_lines = NULL;

    // for every line of input, indexes given line
    while (temp_line_node != NULL) {
        lines_head = remove_front(lines_head);
        index_single_line(temp_line_node->text, ex_words_head, &results);
        temp_line_node = lines_head;
    }
   
   return results;
}

/*
 * Compares 2 IndexedSentences; for qsort
 */
int compare (const void * a, const void * b) {
    char * word_a = (*(IndexedSentence **)a)->indexed_word;
    char * word_b = (*(IndexedSentence **)b)->indexed_word;

    return strncmp(word_a, word_b, strlen(word_a));
}

/*
 * Prints and formats the output BEFORE the indexed word
 */
void print_format_before_word (char * before) {
    int i;
    int length = strlen(before);
    char * output;

    // if the whole string fits prints it
    if (length < 21) {

        // prints the required spaces to indent properly
        for (i = 8; i < 29 - length; i++) {
            printf(" ");
        }
        printf("%s", before);

    // if the whole string does not fit
    } else {

        // finds the first occurence of a space
        output = strchr(&before[length - 21], ' ');

        // prints the required spaces to indent properly
        for(i = 0; i < 21 - strlen(output); i++) {
            printf(" ");
        }

        printf("%s", output);
    }

    free(before);
}

/*
 * Prints and formats the output AFTER the indexed word
 */
void print_format_after_word (char * after, int index_word_length) {
    int i;
    int length = strlen(after);

    // if the whole string fits
    if (length < 32 - index_word_length) {

        i = length;

    // otherwise
    } else {

        // couting backwards looks for the first space
        for (i = 31 - index_word_length; i > 0 && after[i] != ' '; i--) {}
    }

    // remove trailing spaces
    for (; i > 0 && after[i - 1] ==  ' '; i--) {}
    after[i] = '\0';

    printf("%s", after);

    free(after);
}

/*
 * Formats and Prints the sorted Indexed Sentences
 */
void print_results (IndexedArray * results) {
    int i;
    IndexedSentence * line;
    char *word;

    // for every output line
    for (i = 0; i < results->curr_size; i++) {

        line = (results->indexed_lines)[i];
        word = line->indexed_word;
        
        // prints the first 9 spaces, as this is constant
        printf("        ");

        // prints words before indexed word
        print_format_before_word(line->before_word);

        // prints indexed word
        printf("%s", word);

        // prints words after indexed word
        print_format_after_word(line->after_word, strlen(word));
        printf("\n");

        free(word);
        free(line);
    }
}

/*
 * Frees all node_t elements in an array list
 */
void free_list (node_t * head) {
    node_t * temp;

    temp = head;
    while (temp != NULL) {
        head = remove_front(head);
        free(temp->text);
        free(temp);
        temp = head;
    }
}
