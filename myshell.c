#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>

#define MAX_INPUT 1024
#define MAX_ARGS 100

// List of custom command names for auto-completion
const char* custom_commands[] = {
    "me", "goto", "list", "makefolder", "delfolder", "remove", "copy", "move", "create", "findfile", "locatefile",
    "treeview", "show", "revshow", "showmore", "showless", "firstlines", "lastlines", "write", "edit", "open",
    "permchange", "ownchange", "grpchange", "showprocs", "liveprocs", "killid", "killname", "priority", "netping",
    "get", "curlit", "netconfig", "ipconfig", "dnsquery", "amI", "showusers", "userid", "mygroups", "adduser",
    "moduser", "changepass", "switch", "super", "space", "usedir", "blklist", "partition", "format", "scanfs",
    "install", "dpkgtool", "packman", "zipit", "unzipit", "tarit", "gzit", "decompress", "yesman", "pause",
    "clearit", "clock", "calendar", "bye", "quit", "runscript", "schedule", NULL
};

// ====== Function Prototypes ======
void execute_command(char *input);
void run_script(const char* filename);
void schedule_command(int delay, char *command);

// ====== Command Translator ======
char* translate_command(char* cmd) {
    if (strcmp(cmd, "me") == 0) return "pwd";
    if (strcmp(cmd, "goto") == 0) return "cd";
    if (strcmp(cmd, "list") == 0) return "ls";
    if (strcmp(cmd, "makefolder") == 0) return "mkdir";
    if (strcmp(cmd, "delfolder") == 0) return "rmdir";
    if (strcmp(cmd, "remove") == 0) return "rm";
    if (strcmp(cmd, "copy") == 0) return "cp";
    if (strcmp(cmd, "move") == 0) return "mv";
    if (strcmp(cmd, "create") == 0) return "touch";
    if (strcmp(cmd, "findfile") == 0) return "find";
    if (strcmp(cmd, "locatefile") == 0) return "locate";
    if (strcmp(cmd, "treeview") == 0) return "tree";
    if (strcmp(cmd, "show") == 0) return "cat";
    if (strcmp(cmd, "revshow") == 0) return "tac";
    if (strcmp(cmd, "showmore") == 0) return "more";
    if (strcmp(cmd, "showless") == 0) return "less";
    if (strcmp(cmd, "firstlines") == 0) return "head";
    if (strcmp(cmd, "lastlines") == 0) return "tail";
    if (strcmp(cmd, "write") == 0) return "nano";
    if (strcmp(cmd, "edit") == 0) return "vim";
    if (strcmp(cmd, "open") == 0) return "gedit";
    if (strcmp(cmd, "permchange") == 0) return "chmod";
    if (strcmp(cmd, "ownchange") == 0) return "chown";
    if (strcmp(cmd, "grpchange") == 0) return "chgrp";
    if (strcmp(cmd, "showprocs") == 0) return "ps";
    if (strcmp(cmd, "liveprocs") == 0) return "top";
    if (strcmp(cmd, "killid") == 0) return "kill";
    if (strcmp(cmd, "killname") == 0) return "killall";
    if (strcmp(cmd, "priority") == 0) return "nice";
    if (strcmp(cmd, "netping") == 0) return "ping";
    if (strcmp(cmd, "get") == 0) return "wget";
    if (strcmp(cmd, "curlit") == 0) return "curl";
    if (strcmp(cmd, "netconfig") == 0) return "ifconfig";
    if (strcmp(cmd, "ipconfig") == 0) return "ip";
    if (strcmp(cmd, "dnsquery") == 0) return "dig";
    if (strcmp(cmd, "amI") == 0) return "whoami";
    if (strcmp(cmd, "showusers") == 0) return "who";
    if (strcmp(cmd, "userid") == 0) return "id";
    if (strcmp(cmd, "mygroups") == 0) return "groups";
    if (strcmp(cmd, "adduser") == 0) return "useradd";
    if (strcmp(cmd, "moduser") == 0) return "usermod";
    if (strcmp(cmd, "changepass") == 0) return "passwd";
    if (strcmp(cmd, "switch") == 0) return "su";
    if (strcmp(cmd, "super") == 0) return "sudo";
    if (strcmp(cmd, "space") == 0) return "df";
    if (strcmp(cmd, "usedir") == 0) return "du";
    if (strcmp(cmd, "blklist") == 0) return "lsblk";
    if (strcmp(cmd, "partition") == 0) return "fdisk";
    if (strcmp(cmd, "format") == 0) return "mkfs";
    if (strcmp(cmd, "scanfs") == 0) return "fsck";
    if (strcmp(cmd, "install") == 0) return "apt";
    if (strcmp(cmd, "dpkgtool") == 0) return "dpkg";
    if (strcmp(cmd, "packman") == 0) return "yum";
    if (strcmp(cmd, "zipit") == 0) return "zip";
    if (strcmp(cmd, "unzipit") == 0) return "unzip";
    if (strcmp(cmd, "tarit") == 0) return "tar";
    if (strcmp(cmd, "gzit") == 0) return "gzip";
    if (strcmp(cmd, "decompress") == 0) return "gunzip";
    if (strcmp(cmd, "yesman") == 0) return "yes";
    if (strcmp(cmd, "pause") == 0) return "sleep";
    if (strcmp(cmd, "clearit") == 0) return "clear";
    if (strcmp(cmd, "clock") == 0) return "date";
    if (strcmp(cmd, "calendar") == 0) return "cal";
    if (strcmp(cmd, "bye") == 0) return "exit";
    return cmd;
}

// ====== Script Runner ======
void run_script(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Script open failed");
        return;
    }

    char line[MAX_INPUT];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || strlen(line) <= 1) continue; // Skip comments and empty lines
        printf("-> %s", line);
        execute_command(strdup(line));
    }

    fclose(file);
}

// ====== Scheduled Execution ======
void schedule_command(int delay, char *command) {
    printf("Scheduling command: '%s' in %d seconds...\n", command, delay);
    sleep(delay);
    execute_command(command);
}

// ====== Command Execution ======
void execute_command(char *input) {
    char *args[MAX_ARGS];
    char *token = strtok(input, " \n");
    int i = 0;

    if (!token) return;

    // Handle script
    if (strcmp(token, "runscript") == 0) {
        token = strtok(NULL, " \n");
        if (token) run_script(token);
        else printf("Usage: runscript <filename>\n");
        return;
    }

    // Handle scheduler
    if (strcmp(token, "schedule") == 0) {
        token = strtok(NULL, " \n");
        if (!token) { printf("Usage: schedule <delay> <command>\n"); return; }
        int delay = atoi(token);
        token = strtok(NULL, "\n"); // Get entire command as string
        if (!token) { printf("Usage: schedule <delay> <command>\n"); return; }
        schedule_command(delay, token);
        return;
    }

    args[i++] = translate_command(token);

    while ((token = strtok(NULL, " \n")) != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
    }
    args[i] = NULL;

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || chdir(args[1]) != 0) {
            perror("cd failed");
        }
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("Execution failed");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("Fork failed");
    }
}

// ====== Autocomplete ======
char* command_generator(const char* text, int state) {
    static int list_index;
    static int len;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while (custom_commands[list_index]) {
        const char* cmd = custom_commands[list_index++];
        if (strncmp(cmd, text, len) == 0) {
            return strdup(cmd);
        }
    }

    return NULL;
}

char** custom_completion(const char* text, int start, int end) {
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, command_generator);
}

// ====== Main Loop ======
int main() {
    rl_attempted_completion_function = custom_completion;

    while (1) {
        char* input = readline("mysh> ");
        if (!input) break;

        if (*input) {
            add_history(input);

            if (strncmp(input, "quit", 4) == 0 || strncmp(input, "bye", 3) == 0) {
                free(input);
                break;
            }

            execute_command(input);
        }

        free(input);
    }

    printf("Exiting custom shell.\n");
    return 0;
}