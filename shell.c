#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>

void safe(int test) {
    if (test < 0) {
        perror("");
        exit(1);
    }
}

void error(const char* msg) {
    fprintf(stderr, "error: %s", msg);
    exit(1);
}

int run(char* args[], int input, int output) {
    //fprintf(stderr,"'%s'", args[0]);
    int pid = fork();
    safe(pid);
    if (pid == 0) {
        if (input != 0) {
            close(0);
            dup2(input, 0);
        }
        if (output != 1) {
            close(1);
            dup2(output, 1);
        }
        safe(execvp(args[0], args));
    } else {
        if (input != 0) {
            close(input);
        }
        if (output != 1) {
            close(output);
        }
        return pid;
    }
    assert(false);
}

int wait_child(int pid) {
    int status;
    waitpid(pid, &status, 0);
    return status;
}

enum Command { NONE, EXEC, SUBSHELL };

int last_status = 0;
int last_pid = -1;
char* token;
char* cmd;
char* argv[1000];
int argc = 0;
int input = 0;
int output = 1;
enum Command command = NONE;
int pids[1000];
int pid_num = 0;

void execute() {
    if (command == NONE) {
        error("unexpected ';'");
    }
    int pid;
    if (command == EXEC) {
        argv[argc] = NULL;
        pid = run(argv, input, output);
    } else {
        pid = subshell(cmd, input, output);
    }
    argc = 0;
    command = NONE;
    input = 0;
    output = 1;
    pids[pid_num] = pid;
    ++pid_num;
}

int wait_all() {
    int stat;
    for (int i = 0; i < pid_num; ++i) {
        stat = wait_child(pids[pid_num]);
    }
    pid_num = 0;
    return stat;
}

void skip_statement(char** line) {
    int balance = 0;
    while (*line[0] != '\0' && (balance > 0 || (
            *line[0] != '&' && *line[0] != '|' && *line[0] != ';'))) {
        if (*line[0] == '(') {
            ++balance;
        } else if (*line[0] == ')') {
            --balance;
        }
        ++*line;
    }
    while (*line[0] != '\0' && *line[0] != ' ') {
        ++*line;
    }
    while (*line[0] != '\0' && *line[0] == ' ') {
        ++*line;
    }
}

int execute_statement(char* line) {
    last_status = 0;
    last_pid = -1;
    argc = 0;
    input = 0;
    output = 1;
    command = NONE;
    while ((token = strsep(&line, " "))) {
        if (!token[0]) {
            continue;
        }
        if (strcmp(token, "(") == 0) {
            if (command == EXEC) {
                error("unexpected subshell call");
            }
            command = SUBSHELL;
            int balance = 1;
            cmd = line;
            while (line[0] != '\0' && balance > 0) {
                if (line[0] == '(') {
                    ++balance;
                } else if (line[0] == ')') {
                    --balance;
                }
                ++line;
            }
            if (balance != 0) {
                error("unmatched '('");
            }
            line[-1] = '\0';
        } else if (strcmp(token, "cd") == 0) {
            char* path = strsep(&line, " ");
            char* real_path = realpath(path, NULL);
            safe(chdir(real_path));
        } else if (strcmp(token, "exit") == 0) {
            token = strsep(&line, " ");
            exit(atoi(token));
        } else if (strcmp(token, "<") == 0) {
            char* path = strsep(&line, " ");
            input = open(path, O_RDONLY);
        } else if (strcmp(token, ">>") == 0) {
            char* path = strsep(&line, " ");
            output = open(path, O_WRONLY | O_CREAT | O_APPEND, 0777);
        } else if (strcmp(token, ">") == 0) {
            char* path = strsep(&line, " ");
            output = open(path, O_WRONLY | O_CREAT, 0777);
        } else if (strcmp(token, ";") == 0) {
            execute();
            last_status = wait_all();
        } else if (strcmp(token, "&&") == 0) {
            execute();
            last_status = wait_all();
            if (last_status != 0) {
                skip_statement(&line);
            }
        } else if (strcmp(token, "||") == 0) {
            execute();
            last_status = wait_all();
            if (last_status == 0) {
                skip_statement(&line);
            }
        } else if (strcmp(token, "|") == 0) {
            int fildes[2];
            safe(pipe(fildes));
            output = fildes[1];
            execute();
            output = 1;
            input = fildes[0];
        } else {
            if (command == SUBSHELL) {
                error("unexpected word");
            }
            command = EXEC;
            argv[argc] = token;
            ++argc;
        }
    }
    if (command != NONE) {
        execute();
    }
    if (pid_num > 0) {
        last_status = wait_all();
    }
    return last_status;
}

int subshell(char* line, int _input, int _output) {
    int pid = fork();
    safe(pid);
    if (pid == 0) {
        if (_input != 0) {
            close(0);
            dup2(_input, 0);
        }
        if (_output != 1) {
            close(1);
            dup2(_output, 1);
        }
        exit(execute_statement(line));
    } else {
        if (_input != 0) {
            close(input);
        }
        if (_output != 1) {
            close(output);
        }
        return pid;
    }
    assert(false);
}

int main(int argc, char* argv[]) {
   char buf[1024];
   while (1) {
        char* pwd = get_current_dir_name();
        printf("%s > ", pwd);
        fgets(buf, 1024, stdin);
        int i = 0;
        while (buf[i] != '\n') { ++i; }
        buf[i] = '\0';
        if (i > 0 && buf[i - 1] == '&') {
            buf[i - 1] = '\0';
            subshell(buf, 0, 1);
        } else {
            int status = execute_statement(buf);
            if (status != 0) {
                printf("[%i] ", status);
            }
        }
   }
}
