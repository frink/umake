#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
/* Argument Parsing
 * line	  The command line to parse
 * This function returns a new array of pointers that point to characters in
 * line
 */
char** arg_parse(char* line, int *argcp);
