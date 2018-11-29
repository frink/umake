/* CSCI 347 micro-make
 *
 * 09 AUG 2017, Aran Clauson
 */

/*
 * Tony Dinh,
 * Fall 2018
 */

#include "target.h"
#include "arg_parse.h"

/* CONSTANTS */

static Node* head = NULL;

/* PROTOTYPES */

/* Process Targets
 * node     Node with the target matching user input
 *
 * This function recursively runs the dependecies of the given node by
 * processing the targets only if the dependencies are no longer targets
 */
void processTargets(Node* head);

/* Process Line
 * line   The command line to execute.
 *
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete.
 * line is modified, nulls are inserted at the end of arguments
 */
void processline(char* line);

/* Expand
 * orig    The input string that may contain variables to be expanded
 * new     An output buffer that will contain a copy of orig with all 
 *         variables expanded
 * newsize The size of the buffer pointed to by new.
 * returns 1 upon success or 0 upon failure. 
 *
 * Example: "Hello, ${PLACE}" will expand to "Hello, World" when the environment
 * variable PLACE="World". 
 */
int expand(char* orig, char* new, int newsize);

/* Type of Line 
 * line   The command line to be checked
 *
 * Determines the line type, either environment variable, target, or rule.
 *
 */
static int typeOfLine(char* line);

/* Process Environment
 * enviro   variable to be processed into the corresponding environment variable
 *
 * Changes the given variable in the corresponding environment variable using
 * strtok to tokenize the line
 */
static void processEnviro(char* enviro);

/* Main entry point.
 * argc    A count of command-line arguments
 * argv    The command-line argument values
 *
 * Micro-make (umake) reads from the uMakefile in the current working
 * directory.  The file is read one line at a time. Lines without a leading
 * space are interpreted as a target, and lines with a leading tab character
 * ('\t') are interpreted as a command. Nodes are then created with these
 * attributes, as to access later in the file when reading the cmd-line args.
 *
 * The arguments in argv are then used to determine (by target name) which
 * of the rules are going to be ran through processline.
 */
int main(int argc, const char* argv[]) {

  FILE* makefile;
  if((makefile = fopen("./uMakefile", "r")) == NULL) {
    fprintf(stderr, "uMakefile cannot be found. :[\n");       
    exit(1);
  }


  size_t  bufsize = 0;
  char*   line    = NULL;
  ssize_t linelen = getline(&line, &bufsize, makefile);
  char* enviro = NULL;

  while(-1 != linelen) {

    if(line[linelen-1]=='\n') {
      linelen -= 1;
      line[linelen] = '\0';
    }

    if(typeOfLine(line) == 0)
        nodeAppend(&head, line);

    if(typeOfLine(line) == 1)
        processEnviro(line);

    if(typeOfLine(line) == 2) {
        Node* curr = getLastNode(head);
        strListAppend(&curr->rules, line);
    }

    linelen = getline(&line, &bufsize, makefile);
  }

  int i = 1;
  while(argv[i] != NULL) {

     Node* curr = head;

     while(curr != NULL) {

        if(strcmp(curr->target, argv[i]) == 0) {
           processTargets(curr); 
        }
        curr = curr->next;
     }
    ++i;
  }

  freeNodeData(head);
  free(enviro);
  free(line);
  return EXIT_SUCCESS;
}

/* Process Targets
 * node     Node with the target matching user input
 *
 * Finds the node with the matching target, if the findNode function returns
 * null, it will proceed to send the line through processline and run as usual.
 */

void processTargets(Node* node) {

    struct stat targInfo;
    struct stat depInfo;
    unsigned newerThan = 0;

     
    if(node != EMPTY) {
   
        Node* curr = node;
        strNode* currDepNode = curr->dependencies;

        while(currDepNode != NULL) {

            processTargets(nodeFind(head, currDepNode->string));

            if(stat(currDepNode->string, &depInfo) != -1) {
                
                stat(curr->target, &targInfo);

                if(difftime(targInfo.st_mtime, depInfo.st_mtime) < 0)
                    newerThan = 1;
            }

            currDepNode = currDepNode->next;
        }

        if(stat(curr->target, &targInfo) == -1 ||
           newerThan == 1) { 

            strNode* currRuleNode = curr->rules;

            while(currRuleNode != NULL) {
                char* rules = strdup(currRuleNode->string);
                processline(rules);
                currRuleNode = currRuleNode->next;
            }
        }

    }

    return;
}

/* Process Line
 * line     Line to be processed
 *
 * Returns immediately if line is an empty line
 */
void processline (char* line) {
  
  int newsize = 1024;
  char new[newsize];
  int env = expand(line, new, newsize);

  char** arglist;
  int argcount = 0;

  if(env == 0) {
    fprintf(stderr, "Error: in environment var syntax.\n");
    exit(1);
  }
  
  arglist = arg_parse(new, &argcount);

  if(argcount != 0) {

      const pid_t cpid = fork();

      switch(cpid) {

      case -1: {
        perror("fork");
        break;
      }

      case 0: {

        int i = 0;
        while(arglist[i] != NULL) {
    
            if(strcmp(arglist[i], "<") == 0) {

                int fd1 = open(arglist[i+1], O_RDONLY);

                if(fd1 < 0)
                    perror("open");

                int j = 0;
                while(j != argcount-1) {
                
                    if(j >= i)
                        arglist[j] = arglist[j+1];
                    
                    j++;
                }

                arglist[j] = '\0';
        
                dup2(fd1, 0);
                close(fd1);
            }

            else if(strcmp(arglist[i], ">") == 0) {

                int fd2 = open(arglist[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

                if(fd2 < 0)
                    perror("open");

                arglist[i] = '\0';
                dup2(fd2, 1);
                close(fd2);
            }

            else if(strcmp(arglist[i], ">>") == 0) {
                
                int fd3 = open(arglist[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);

                if(fd3 < 0)
                    perror("open");

                arglist[i] = '\0';
                dup2(fd3, 1);
                close(fd3);
            }

            i++;
        }

        execvp(arglist[0], arglist);
        perror("execvp");
        free(arglist);
        exit(EXIT_FAILURE);
        break;
      }

      default: {
        int   status;
        const pid_t pid = wait(&status);
        if(-1 == pid) {
            perror("wait");
        }
        else if (pid != cpid) {
          fprintf(stderr, "wait: expected process %d, but waited for process %d",
                  cpid, pid);
        }
        break;
      }
      }

      free(arglist);
  }

  return;
}

/* Expand
 * orig    The input string that may contain variables to be expanded
 * new     An output buffer that will contain a copy of orig with all 
 *         variables expanded
 * newsize The size of the buffer pointed to by new.
 * returns 1 upon success or 0 upon failure. 
 *
 * Parses the line charactor by charactor and if and only if the line has a
 * syntactically correct environment variable(s), it will return the line with
 * the corresponding environment variable. 
 */
int expand(char* orig, char* new, int newsize) {


    int arg_count = 0;
    char** args = arg_parse(orig, &arg_count);
    char env[1024];
    char* getEnv;

    int correct = 0;

    int i = 0;
    int newIncr = 0;
    while(args[i] != NULL) {
        
        if(args[i][0] == '$') {

            if(args[i][1] != '{')
                return 0;

            int t = 0;
            while(args[i][t+2] != '\0') {

                if(args[i][t+2] == '{' ||
                   args[i][t+2] == '$')
                    return 0;

                if(args[i][t+2] == '}') {
                    correct = 1;
                    break;
                }

                env[t] = args[i][t+2];
                t++;
            }

            env[t] = '\0';

            if(correct == 0)
                return 0;
           
            if(getenv(env) == NULL)
                return 0;

            getEnv = getenv(env);

            int j = 0;
            while(getEnv[j] != '\0') {
                new[newIncr] = getEnv[j];
                newIncr++;
                j++;
            }

            new[newIncr++] = ' ';
        }

        else {
            
            int j = 0;
            while(args[i][j] != '\0') {
                new[newIncr] = args[i][j];
                newIncr++;
                j++;
            }

            new[newIncr++] = ' ';
        }
        i++;
    }

    new[newIncr++] = '\0';
    free(args);
    return 1;
}

/* Type of Line 
 * line   The command line to be checked
 *
 * Checks if the line contains certain characters that describe each type of
 * line.
 *
 * Change this to a state machine man...
 *
 */
static int typeOfLine(char* line) {
    
    int i = 0;
    while(line[i] != '\0') {
        
        if(line[i] == '#')
            break;
        
        i++;
    }

    line[i] = '\0';

    if(line[0] == '\t') {
        return 2;
    }

    if(strchr(line, ':') != NULL) {
        return 0;
    }
    if(strchr(line, '=') != NULL) {
        return 1;
    }
    return -1;
}

/* Process Environment
 * enviro   variable to be processed into the corresponding environment variable
 *
 * Tokenizes the line and sets the environment variable to be equal to the
 * corresponding user choice. String must be in form "X=Y".
 */
static void processEnviro(char* line) {
    
    char* enviro = strdup(line);
    char* token; 
    char* name;
    char* value;
    token = strtok(enviro, "=");

    int nm = 0;
    int word;
    while(token != NULL) {
      
        if(nm == 0) {
            nm = 1;
            name = strdup(token);

            int j = 0;
            int k = 0;
            while(token[j] != '\0') {
            
                if(!isspace(token[j])) {
                    name[k] = token[j];
                    k++;
                }

                j++;
            }

            name[k] = '\0';
        }

        else {
            value = strdup(token);
            
            int j = 0;
            int k = 0;
            while(token[j] != '\0') {
            
                if(!isspace(token[j]))
                    word = 1;

                if(word == 1) {
                    value[k] = token[j];
                    k++;
                }

                j++;
            }

            value[k] = '\0';
        }

        token = strtok(NULL, "=");
    }

    setenv(name, value, 1);
}


