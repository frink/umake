#include "arg_parse.h"

/* Count Arguments
 * line     The line to count
 *
 * Counts the amount of arguments in the uMakeFile in order to allocate only
 * the necessary memory
 */
static unsigned int count_arguments(char* line);

/* Count Arguments
* line      The line to count
* This function traverses the argument and only adds to
* count when it comes across a character that is not a
* ' ' or a '\t'. It will only begin to look for another word
* only when a ' ' or a '\t' shows up, indicating the end of the word
*/
static unsigned int count_arguments(char* line) {
    
    unsigned count = 0;
    unsigned word = 0;
    while(*line) {

        if(isspace(*line)) //Marks either the end of a word 
            word = 0;      //or a space character

        else if (word == 0) { //Start of word
            word = 1;
            ++count;
        }
        ++line;
    }

    return count;
}

/* Argument Parsing
 * line     The command line to parse
 *
 * This function uses similar logic to traverse the
 * string as in count_arguments. The differences are:
 * - This doesn't increment line itself.
 * - When a word is found, the current index of args
 * is set to the address of the character at that index.
 */
char** arg_parse(char* line, int *argcp) {
    assert(argcp != NULL);
    
    unsigned count = count_arguments(line) + 1;
    char** args = malloc (count * sizeof(char*));
    *argcp = count - 1;
    
    unsigned argsc = 0;
    unsigned i = 0;
    unsigned word = 0;
    while(line[i] != '\0') { 

        if(isspace(line[i])) { //Marks either the end of a word
            line[i] = '\0';    //or a space character
            word = 0;
        }

        else if(word == 0) { //Start of word
            word = 1;
            args[argsc] = &line[i];
            ++argsc;
        }
        ++i;
    }
    args[count - 1] = NULL;

    return args;
}
