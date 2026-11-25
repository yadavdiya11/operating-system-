#ifndef ALGORITHMS_H
#define ALGORITHMS_H

/* Result of one disk-scheduling run. Free with result_free(). */
typedef struct {
    int  *path;    /* cylinder visited at each step            */
    int  *seek;    /* seek distance of each step               */
    int  *served;  /* 1 = real request, 0 = boundary/jump move */
    int   steps;   /* number of steps recorded                 */
    long  total;   /* total head movement                      */
    double avg;    /* total / number of requests               */
    int   n;       /* number of requests                       */
} Result;

enum { FCFS, SSTF, SCAN, CSCAN, LOOK, CLOOK, ALG_COUNT };

/* dir: 1 = right (increasing), 0 = left.
 * Return 0 on success, <0 on error:
 *  -1 invalid range/head/input, -2 request out of range,
 *  -3 out of memory,            -4 unknown algorithm id */
int  fcfs (const int*r,int n,int head,int lo,int hi,int dir,Result*o);
int  sstf (const int*r,int n,int head,int lo,int hi,int dir,Result*o);
int  scan (const int*r,int n,int head,int lo,int hi,int dir,Result*o);
int  cscan(const int*r,int n,int head,int lo,int hi,int dir,Result*o);
int  look (const int*r,int n,int head,int lo,int hi,int dir,Result*o);
int  clook(const int*r,int n,int head,int lo,int hi,int dir,Result*o);
int  run_algorithm(int id,const int*r,int n,int head,int lo,int hi,int dir,Result*o);
const char *algorithm_name(int id);
void result_free(Result*o);
#endif
