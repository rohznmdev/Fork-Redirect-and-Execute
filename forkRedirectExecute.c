#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int argSizeCalc(char * userArguments[]);
/**
 *
 * Checks the length of the userArguments.
 *
 */
int argSizeCalc(char * userArguments[]) {
    // setting up variables.
    int l = 0;
    for (int i = 0; i < 4096; i++) {
        // checking to see if the argument is null
        if (userArguments[i] != NULL) {
            // increments if it is not null.
            l++;
        } // if
    } // for
    // return the length.
    return l;
} // argSizeCalc

/**
 * Represents the main method which is responsible for carrying out all functions.
 *
 *
 */
int main (int argc, char* argv[]) {
    // setting up variables.
    pid_t pid;
    char * userArguments[4096];
    char * delimiter = " \n";
    char prompt[] = "Please enter command: ";
    int promptSize = strlen(prompt);
    int out = dup(STDOUT_FILENO);
    int in = dup(STDIN_FILENO);

    // while the condition is true
    while (1) {
        // setting up while variables.
        int buffSz = 4096;
        char buff[buffSz];
        // setting the arguments to \0
        for (int i = 0; i < buffSz; i++) {
            userArguments[i] = '\0';
            buff[i] = '\0';
        } // for

        dup2(out, 1);
        dup2(in, 0);

        // asking the prompt
        write(STDOUT_FILENO, prompt, promptSize);

        // reading the input from the prompt
        int r = read(STDIN_FILENO, buff, buffSz);
        char * token = calloc(r, sizeof(char));
        token = strtok(buff, delimiter);

        int j = 0;

        // checks for a vaild argument
        while (token != NULL) {
            userArguments[j] = token;
            // checks if the argument is exit.
            if (strcmp("exit", token) == 0) {
                exit(0);
            } // if
            // changing the value of the token and incrementing the argument to be checked
            // for the next run.
            token = strtok(NULL, delimiter);
            j++;
        } // while

        // calculating the argument length.
        int argLen = argSizeCalc(userArguments);

        // going through the arguments to see if > >> or < is present to determine whether to
        // append or do another command.
        for (int i = 0; i < argLen; i++) {
            if (strcmp(userArguments[i], ">") == 0) {
                char * path = userArguments[i + 1];
                int op = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(op, STDOUT_FILENO);
            } else if (strcmp(userArguments[i], ">>") == 0) {
                char * path = userArguments[i + 1];
                int op = open(path, O_WRONLY | O_CREAT | O_APPEND, 0666);
                dup2(op, STDOUT_FILENO);
            } else if (strcmp(userArguments[i], "<") == 0) {
                char * path = userArguments[i + 1];
                int op = open(path, O_RDONLY);
                dup2(op, STDIN_FILENO);
            } // if
        } // for
        // logic to check for whether to break out of the loop.
        for (int i = 0; i < argLen; i++) {
            if ((strcmp(userArguments[i], ">") == 0) | (strcmp(userArguments[i], "<") == 0) |
            (strcmp(userArguments[i], ">>") == 0)) {
                userArguments[i] = NULL;
                break;
            } // if
        } // for
        // using the fork
        pid = fork();
        // checks to see if it should execute the argument.
        if (pid == 0) {
            execvp(userArguments[0], userArguments);
        } else {
            wait(NULL);
        } // if else

        // returns if there is an error.
        if (pid == -1) {
            perror("ERROR");
            return 1;
        } // if
    } // while
} // main
