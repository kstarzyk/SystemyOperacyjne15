// Includes
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define
#define KSS_BUFF_SIZE 64
#define KSS_DELIMITERS " \t\r\n\a"

// Macros
#define FAILURE exit(EXIT_FAILURE)
// Config
char *USER = "kstarzyk";
int VERBOSE = 1;

// Builts
char *builtin_cmd[] = {
  "cd",
  "exit"
};

int KSS_cd(char **args);
int KSS_exit(char **args);

int (*builtin_func[]) (char **) = {
  &KSS_cd,
  &KSS_exit
};

int KSS_cd(char **args) {
    if (args[1] == NULL) {
      fprintf(stderr, "KSS: \"cd\" needs argument\n");
    } else {
      if (chdir(args[1]) != 0) {
        perror("KSS");
      }
    }
    return 1;
}

int KSS_exit(char **args)
{
    return 0;
}

int KSS_builtns_num() {
  return sizeof(builtin_cmd) / sizeof(char *);
}

void KSS_about(void)
{
    printf(">>> Krzysztof Starzyk Shell\n");
    printf(">>> For batch mode pass file as argument \n");
    printf(">>> User: %s \n", USER);
}


// Code

char *KSS_read_line(void)
{
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

char **KSS_tokenize(char *line)
{

    int bufsize = KSS_BUFF_SIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "KSS: allocation error\n");
        FAILURE;
    }

    token = strtok(line, KSS_DELIMITERS);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += KSS_BUFF_SIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "KSS: allocation error\n");
                FAILURE;
            }
        }

        token = strtok(NULL, KSS_DELIMITERS);
    }

    tokens[position] = NULL;
    return tokens;
}

int KSS_execute(char **args)
{
    int i;

    if (args[0] == NULL) {
      return 1;
    }

    for (i = 0; i < KSS_builtns_num(); i++) {
        if (strcmp(args[0], builtin_cmd[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return KSS_launch(args);
}

void KSS_loop(void)
{
    char *line;
    char **args;
    int status = 1;
    char DIR[1024];

    getcwd(DIR, sizeof(DIR));

    while(status) {
        printf("> %s@%s$ ", USER, DIR);
        line = KSS_read_line();
        args = KSS_tokenize(line);
        status = KSS_execute(args);
	    getcwd(DIR, sizeof(DIR));
        free(line);
        free(args);
    }
}

int KSS_launch(char **args)
  {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
    // Child
        if (execvp(args[0], args) == -1) {
            perror("KSS");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
    // Error
        perror("KSS");
    } else {
    // Parent
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

void KSS_batch(char* filename)
{
    printf("Process batch file: %s \n", filename);

    FILE *file = fopen ( filename, "r" );
    if ( file != NULL ) {
        char line [ 1024 ];
        char **args;
        int status = 1;

        while ( fgets ( line, sizeof line, file ) != NULL && status )
        {
        if (VERBOSE) {
            printf("%s <<< ", filename);
            fputs ( line, stdout );
        }
       args = KSS_tokenize(line);
       status = KSS_execute(args);

       free(args);
     }
     fclose ( file );
   } else {
      perror ( filename );
   }
   return;
}


int main(int argc, char **argv)
{
    // batch mode
    if (argc > 1) {
        KSS_batch(argv[1]);
    } else { // interactive mode
        KSS_about();
        KSS_loop();
    }

    return EXIT_SUCCESS;
}
