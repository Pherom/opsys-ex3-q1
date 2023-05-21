#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define GRADES_FILE "all_std.log"
#define ERR_OUT_WRITE "Failed to open output file for writing"
#define ERR_IN_READ "Failed to open input file for reading"
#define PART_MSG_FORMAT "process: %d file: %s number of students: %zu\n"
#define FINAL_MSG_FORMAT "grade calculation for %d students is done\n"

char* getTempFileName(int pid);
void getStdNameAndGradeAvg(char* line, char std_name[11], float* grade_avg);
void processFile(char* in_file_name);
void createAllStdFile(int* pid_arr, size_t pid_count);
void report_data_summary(int num_stud);

int main(int argc, char* argv[]) {
    int pid;
    int* pid_arr;
    int wstatus;
    int pid_index = 0;
    int std_count = 0;

    for (int i = 1; i < argc; i++) {
        pid = fork();
        if (pid == 0) {
            processFile(argv[i]);
            break;
        }
    }

    //Unknown number of file names as arguments so dynamic allocation used
    pid_arr = (int*)malloc(sizeof(int) * (argc - 1));

    while((pid = wait(&wstatus)) != -1) {
        pid_arr[pid_index] = pid;
        std_count += WEXITSTATUS(wstatus);
        pid_index++;
    }

    pid = fork();
    if (pid == 0) {
        createAllStdFile(pid_arr, argc - 1);
    }

    if (pid != 0) {
        wait(NULL);
        report_data_summary(std_count);
        free(pid_arr);
    }
}

char* getTempFileName(int pid) {
    //Temp file name dependant on length of pid therefore dynamic allocation used
    char* temp_file_name = (char*)malloc((int)((ceil(log10(pid))+6)*sizeof(char)));
    if (temp_file_name == NULL) {
        perror("Failed to allocate memory for output file name");
    }
    else {
        sprintf(temp_file_name, "%d.temp", pid);
    }
    return temp_file_name;
}

void getStdNameAndGradeAvg(char* line, char std_name[11], float* grade_avg) {
    size_t grade_count = 0;
    size_t grade_sum = 0;
    char* token;

    strcpy(std_name, strtok(line, " \t"));
    token = strtok(NULL, " \t");
    while (token != NULL) { 
        grade_sum += atoi(token);
        grade_count++;
        token = strtok(NULL, " \t");
    }

    *grade_avg = (float)grade_sum / grade_count;
}

void processFile(char* in_file_name) {
    FILE* in_file = fopen(in_file_name, "r");
    FILE* out_file;
    char* out_file_name;
    char* curr_line = NULL;
    char std_name[11];
    float grade_avg;
    size_t curr_line_len;
    size_t std_count = 0;
    int pid = getpid();

    if (in_file == NULL) {
        perror(ERR_IN_READ);
    }
    else {
        out_file_name = getTempFileName(pid);

        if (out_file_name != NULL) {
            out_file = fopen(out_file_name, "w");

            if (out_file == NULL) {
                perror(ERR_OUT_WRITE);
            }
            else {
                while(getline(&curr_line, &curr_line_len, in_file) != -1) {
                    getStdNameAndGradeAvg(curr_line, std_name, &grade_avg);
                    if (std_count != 0) {
                        fwrite("\n", sizeof(char), 1, out_file);
                    }
                    fprintf(out_file, "%s %.1f", std_name, grade_avg);
                    free(curr_line);
                    std_count++;
                }
                fclose(out_file);
            }

            free(out_file_name);
        }

        fclose(in_file);
    }

    fprintf(stderr, PART_MSG_FORMAT, pid, in_file_name, std_count);
    
    exit(std_count);
}

void createAllStdFile(int* pid_arr, size_t pid_count) {
    char* in_file_name;
    char* curr_line;
    FILE* in_file;
    FILE* out_file;
    size_t curr_line_len;

    out_file = fopen(GRADES_FILE, "w");
    
    if (out_file == NULL) {
        perror(ERR_OUT_WRITE);
    }

    else {
        for (int i = 0; i < pid_count; i++) {
            in_file_name = getTempFileName(pid_arr[i]);

            if (in_file_name != NULL) {
                in_file = fopen(in_file_name, "r");
                if (in_file == NULL) {
                    perror(ERR_IN_READ);
                }
                else {
                    while(getline(&curr_line, &curr_line_len, in_file) != -1) {
                        fprintf(out_file, "%s", curr_line);
                    }
                    if (i < pid_count - 1) {
                        fprintf(out_file, "\n");
                    }
                    fclose(in_file);
                }
                free(in_file_name);
            }
        }
        fclose(out_file);
    }
}

void report_data_summary(int num_stud) {
    fprintf(stderr, FINAL_MSG_FORMAT, num_stud);
}

