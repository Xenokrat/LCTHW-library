#include <stdio.h>
#include <glob.h>
#include <stdlib.h>
#include "../src/lcthw/dbg.h"
#include <sys/wait.h>
#include <unistd.h> 

const char *test_path = "tests/*_tests";

int run_test(const char *test)
{
    pid_t pid = fork();
    int status = 0;

    if (pid == 0) {
        /* We are in child process */
        execl(test, test, NULL);
        // If we are here execl failed
        perror("waitpid failed");
        exit(1);
    } else if (pid > 0) {
        /* We are in parent process */
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return -1;
        }

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    } else {
        /* Fork failed */
        perror("Fork failed");
        return -1;
    }

    return -1;
}

int main(void)
{
    glob_t globbuf;
    size_t rc = 0;
    size_t i = 0;
    size_t tests_run = 0;
    size_t tests_faield = 0;

    rc = glob(test_path, 0, NULL, &globbuf);
    check(rc == 0, "Error discover tests in path %s", test_path);

    log_info("Found %zu tests in %s", globbuf.gl_pathc, test_path);
    for (i = 0; i < globbuf.gl_pathc; i++) {
        rc = run_test(globbuf.gl_pathv[i]);

        if (rc != 0) {
            log_err("FAILED with exit code %zu", rc);
            tests_faield++;
        } else {
            log_info("PASSED");
        }
        tests_run++;
    }    

    globfree(&globbuf);

    printf("\nResults: %zu tests, %zu failed\n", tests_run, tests_faield);
    return tests_faield > 0 ? 1 : 0;

error:
    globfree(&globbuf);
    return -1;
}
