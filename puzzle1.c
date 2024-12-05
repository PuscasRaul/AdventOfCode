#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stdout, "provide input file and filtered input file\n");
        exit(EXIT_FAILURE);
    }

    int filtered_fd = open(argv[2], O_WRONLY, 0644);
    if (filtered_fd < 0) {
        perror("couldn't open filter file");
        exit(EXIT_FAILURE);
    }

    int pid = fork();

    if (pid < 0) {
        perror("fork failure");
        exit(EXIT_FAILURE);
    }

    if (!pid) {

        if (dup2(filtered_fd, STDOUT_FILENO) < 0) {
            perror("couldn't duplicate file output");
            exit(EXIT_FAILURE);
        }

        char *filter_command = "grep";
        execlp(filter_command, filter_command, "-o", "mul([0-9]*,[0-9]*)",
               argv[1], NULL);
    }

    if (pid) {

        close(filtered_fd);

        int wait_status;
        wait(&wait_status);
        if (wait_status) {
            exit(EXIT_FAILURE);
        }

        FILE *filtered_file = fopen(argv[2], "r");
        if (filtered_file == NULL) {
            perror("couldn't open filtered_fd in parent");
            exit(EXIT_FAILURE);
        }

        char *line = malloc(sizeof(char) * 20);
        if (line == NULL) {
            perror("malloc failure");
            exit(EXIT_FAILURE);
        }

        int32_t total_sum = 0;

        while (fgets(line, 20, filtered_file) != NULL) {
            int n1 = strtol(line + 4, NULL, 10);
            int offset = 0;
            int n2, c_n1 = n1;
            while (c_n1) {
                ++offset;
                c_n1 /= 10;
            }
            n2 = strtol(line + 5 + offset, NULL, 10);
            total_sum += n1 * n2;
        }

        fprintf(stdout, "total_sum: %d\n", total_sum);
    }

    exit(EXIT_SUCCESS);
}
