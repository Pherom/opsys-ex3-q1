#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "ex3_q1_given.h"

#define ERR_IN_READ "Failed to open input file for reading"
#define READ_DONE_MSG_FORMAT "all %d threads terminated\n"
#define PRINT_DONE_MSG_FORMAT "all printer-threads terminated\n"
#define LETTER_GRADE_COUNT 5

struct letter_func {
    print_grade_func func;
    char letter;
    int min_grade;
    int grade_limit;
};

void* processFile(void* in_file_name);
void getStdNameAndGradeAvg(char* line, char std_name[MAX_NAME_LEN + 1], double* grade_avg);
void* processGrade(void* let_func_v);

const print_grade_func functions[LETTER_GRADE_COUNT] = { print_grade_A, print_grade_B, print_grade_C, print_grade_D, print_grade_F };
const char letters[LETTER_GRADE_COUNT] = { 'A', 'B', 'C', 'D', 'F' };
const int min_grades[LETTER_GRADE_COUNT] = { 90, 80, 70, 60, 0 };
const int grade_limits[LETTER_GRADE_COUNT] = { 101, 90, 80, 70, 60 };

struct all_students all_stud;
int printer_index = 0;
int read_threads_running;
int print_msg_waiting = LETTER_GRADE_COUNT;

pthread_mutex_t add_student_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_finish_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t read_finish_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t print_index_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t print_index_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t print_msg_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t print_msg_cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]) {
    pthread_t* read_threads = (pthread_t*)malloc(sizeof(pthread_t) * (argc - 1));
    pthread_t printer_threads[LETTER_GRADE_COUNT];
    

    pthread_mutex_init(&add_student_lock, NULL);
    pthread_mutex_init(&read_finish_lock, NULL);
    pthread_cond_init(&read_finish_cond, NULL);

    read_threads_running = argc - 1;

    for (int i = 1; i < argc; i++) {
        pthread_create(read_threads + i - 1, NULL, processFile, (void*) argv[i]);
    }

    for (int i = 0; i < argc - 1; i++) {
        pthread_join(read_threads[i], NULL);
    }

    pthread_mutex_destroy(&add_student_lock);
    pthread_mutex_destroy(&read_finish_lock);
    pthread_cond_destroy(&read_finish_cond);

    fprintf(stderr, READ_DONE_MSG_FORMAT, argc - 1);
    free(read_threads);

    pthread_mutex_init(&print_msg_lock, NULL);
    pthread_cond_init(&print_msg_cond, NULL);
    pthread_mutex_init(&print_index_lock, NULL);
    pthread_cond_init(&print_index_cond, NULL);

    for (int i = 0; i < LETTER_GRADE_COUNT; i++) {
        struct letter_func* let_func = (struct letter_func*)malloc(sizeof(struct letter_func));
        let_func->func = functions[i];
        let_func->letter = letters[i];
        let_func->min_grade = min_grades[i];
        let_func->grade_limit = grade_limits[i];
        pthread_create(&(printer_threads[i]), NULL, processGrade, (void*) let_func);
    }

    for (int i = 0; i < LETTER_GRADE_COUNT; i++) {
        pthread_join(printer_threads[i], NULL);
    }

    fprintf(stderr, PRINT_DONE_MSG_FORMAT);

    pthread_mutex_destroy(&print_msg_lock);
    pthread_cond_destroy(&print_msg_cond);
    pthread_mutex_destroy(&print_index_lock);
    pthread_cond_destroy(&print_index_cond);
}

void getStdNameAndGradeAvg(char* line, char std_name[MAX_NAME_LEN + 1], double* grade_avg) {
    size_t grade_count = 0;
    size_t grade_sum = 0;
    char* token;

    static char* saveptr;
    strcpy(std_name, strtok_r(line, " \t", &saveptr));
    token = strtok_r(NULL, " \t", &saveptr);
    while (token != NULL) { 
        grade_sum += atoi(token);
        grade_count++;
        token = strtok_r(NULL, " \t", &saveptr);
    }

    *grade_avg = (double)grade_sum / grade_count;
}

void* processFile(void* in_file_name) {
    char* in_file_name_str = (char*)in_file_name;
    FILE* in_file = fopen(in_file_name_str, "r");
    char* curr_line = NULL;
    size_t curr_line_len;

    if (in_file == NULL) {
        perror(ERR_IN_READ);
    }
    else {
        while(getline(&curr_line, &curr_line_len, in_file) != -1) {
            struct student a_student;
            getStdNameAndGradeAvg(curr_line, a_student.name, &(a_student.avg_grade));
            pthread_mutex_lock(&add_student_lock);
            add_to_student_arr(&a_student);
            pthread_mutex_unlock(&add_student_lock);
        }

        free(curr_line);
        fclose(in_file);
    }

    pthread_mutex_lock(&read_finish_lock);
    read_threads_running--;

    if (read_threads_running == 0) {
        print_student_arr();
        pthread_cond_broadcast(&read_finish_cond);
    }

    else {
        while(read_threads_running > 0) {
            pthread_cond_wait(&read_finish_cond, &read_finish_lock);
        }
    }

    pthread_mutex_unlock(&read_finish_lock);
}

void* processGrade(void* let_func_v) {
    struct letter_func* let_func = (struct letter_func*)let_func_v;

    pthread_mutex_lock(&print_msg_lock);
    printer_thread_msg(let_func->letter);
    print_msg_waiting--;

    if (print_msg_waiting == 0) {
        pthread_cond_broadcast(&print_msg_cond);
    }

    else {
        while (print_msg_waiting > 0) {
            pthread_cond_wait(&print_msg_cond, &print_msg_lock);
        }
    }

    pthread_mutex_unlock(&print_msg_lock);

    while (printer_index < all_stud.count) {

        bool handled_by_thread = (all_stud.stud_arr)[printer_index].avg_grade >= let_func->min_grade &&
        (all_stud.stud_arr)[printer_index].avg_grade < let_func->grade_limit;

        pthread_mutex_lock(&print_index_lock);

        if (handled_by_thread) {
            let_func->func(printer_index);
            printer_index++;
            pthread_cond_broadcast(&print_index_cond);
        }

        else {
            while(printer_index < !handled_by_thread) {
                pthread_cond_wait(&print_index_cond, &print_index_lock);
            }
        }

        pthread_mutex_unlock(&print_index_lock);
    }

    free(let_func);
}