/*  file: ex3_q1_given.h
  ****  do not change or modify this file ****
*/

#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdbool.h>

// the following might be needed to avoid multiple includes.
#ifndef ex3_q1_given
#define ex3_q1_given

// *********** type declarations ***********
#define MAX_STUDENTS 100
#define MAX_NAME_LEN 10

struct student {
  char   name[MAX_NAME_LEN + 1];
  double avg_grade; // average of all grades
};

struct all_students {
  int count;    // number of students in stud_arr
  struct student stud_arr[MAX_STUDENTS];
};


// the following is a global variable.
// the real declaration of all_students should be in **your own** program
extern struct all_students all_stud;  

// *********** function prototypes ***********
void add_to_student_arr (struct student*);
void printer_thread_msg(char grade_letter);
void sort_student_arr();
void print_student_arr();

typedef void (*print_grade_func)(int);  // this might help you.

void print_grade_A(int indx);
void print_grade_B(int indx);
void print_grade_C(int indx);
void print_grade_D(int indx);
void print_grade_F(int indx);
void random_sleep();

#endif

