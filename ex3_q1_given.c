#include "ex3_q1_given.h"

// printing MACROS.  You can use the following for debugging if you like
#define DEBUG 1
#define DEB DEBUG
#define DEB_MSG if (DEBUG) \
        {fprintf(stderr, "***DEBUG*** function: %s line: %d\n", __func__, __LINE__);}
#define DEB_MSG1(msg) if (DEBUG) \
        {fprintf(stderr, "***DEBUG*** function: %s line: %d %s\n", __func__, __LINE__, msg);}
#define DEB_MSG_STDOUT if (DEBUG) \
        {fprintf(stdout, "***DEBUG*** function: %s line: %d\n", __func__, __LINE__);}
#define LINE_FUNC if(DEBUG) printf("Line (%d) function: %s\n", __LINE__, __func__);
#define PVI(var) if(DEB) \
        {fprintf(stderr, "***DEBUG %s = %d\n", #var, var);}
#define PVD(var) if(DEB) \
        {fprintf(stderr, "***DEBUG %s = %f\n", #var, var);}
#define PVS(var) if(DEB) \
        {fprintf(stderr, "***DEBUG %s = %s\n", #var,  var);}
#define PVPTR(var) if(DEB) \
        {fprintf(stderr, "***DEBUG %s = %p\n", #var, var);}


//------------------------------------------------------------------------
void add_to_student_arr (struct student* a_student)
{
  random_sleep();
  all_stud.stud_arr[all_stud.count] = *a_student;
  random_sleep();
  all_stud.count = all_stud.count + 1;
}

//------------------------------------------------------------------------
void sort_student_arr()
{
  struct student temp;
  int num = all_stud.count;
  for (int k = 0; k < num - 1 ; k++)
    for (int j = 0; j < num -1 -k; j++)
      if (all_stud.stud_arr[j].avg_grade < all_stud.stud_arr[j+1].avg_grade)
      {
        temp = all_stud.stud_arr[j];
        all_stud.stud_arr[j] = all_stud.stud_arr[j+1];
        all_stud.stud_arr[j+1] = temp;
      }
}

//------------------------------------------------------------------------
void print_student_arr()
{
  static bool first = true;
  if (first) {
    first = false;
    printf("printing students by a single thread\n");
    printf("====================================\n");
  }
  for (int j=0; j < all_stud.count; j++)
  {
    printf("Name: %-10s grade: %.1f\n", 
           all_stud.stud_arr[j].name, all_stud.stud_arr[j].avg_grade);
  }
  fflush(stdout);
}

//------------------------------------------------------------------------
void printer_thread_msg(char grade_letter)
{
  fprintf(stderr, "thread_id = %ld in charge of printing grade %c\n",
  (long unsigned int) pthread_self(), grade_letter);
}


//------------------------------------------------------------------------
void print_student(int indx, const char* msg)
{
  static bool first = true;
  if (first) {
    first = false;
    printf("printing students by five threads\n");
    printf("====================================\n");
  }
  printf("thread_id = %ld %-10s grade: ", (long unsigned) pthread_self(), 
         all_stud.stud_arr[indx].name);
  random_sleep();
  printf("%.1f ***%s***\n", all_stud.stud_arr[indx].avg_grade, msg);
}

//------------------------------------------------------------------------
char* msg[] = {
  "your grade is A :)!! :) !! ",
  "your grade is B :)!! :) ",
  "your grade is C :)!! ",
  "your grade is D !! ",
  "your grade is F (: ",
};

//------------------------------------------------------------------------
void print_grade_A(int indx){
  print_student(indx, msg[0]);
}

void print_grade_B(int indx){
  print_student(indx, msg[1]);
}

void print_grade_C(int indx){
  print_student(indx, msg[2]);
}

void print_grade_D(int indx){
  print_student(indx, msg[3]);
}

void print_grade_E(int indx){
  print_student(indx, msg[4]);
}

void print_grade_F(int indx){
  print_student(indx, msg[4]);
}


//------------------------------------------------------------------------
void random_sleep()
{
  int d = (rand() ^ pthread_self() ^ getpid()) % 40 + 40;
  const struct timespec ts = {0, d * 1000 * 1000 * 5};
  nanosleep(&ts, NULL);
}

