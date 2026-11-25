/* NEXUS command-line interface (interactive menu + argument mode). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "algorithms.h"

static void print_run(int id,const int*r,int n,int head,int lo,int hi,int dir,int verbose){
    Result o; int e=run_algorithm(id,r,n,head,lo,hi,dir,&o);
    (void)verbose;
    if(e){ printf("%-7s : error code %d\n",algorithm_name(id),e); return; }
    printf("\n=== %s ===  total=%ld  avg=%.2f\n",algorithm_name(id),o.total,o.avg);
    printf("path: %d",head);
    for(int i=0;i<o.steps;i++) printf(" -> %d%s",o.path[i],o.served[i]?"":"*");
    printf("\n(* = boundary/jump, not a request)\n\n");
    int W=60, prev=head;
    for(int i=0;i<o.steps;i++){
        int col=(int)((long)(o.path[i]-lo)*(W-1)/(hi-lo));
        char line[128]; memset(line,' ',(size_t)W); line[W]=0;
        line[(int)((long)(prev-lo)*(W-1)/(hi-lo))]='.'; line[col]=o.served[i]?'o':'x';
        printf("%6d |%s| +%d\n",o.path[i],line,o.seek[i]); prev=o.path[i];
    }
    result_free(&o);
}

static void run_all(const int*r,int n,int head,int lo,int hi,int dir,int verbose){
    printf("\nHead=%d  Range=[%d,%d]  Direction=%s  Requests=%d\n",head,lo,hi,dir?"right":"left",n);
    int best=-1; long bt=0; long tot[ALG_COUNT];
    for(int i=0;i<ALG_COUNT;i++){
        Result o; if(run_algorithm(i,r,n,head,lo,hi,dir,&o)){ tot[i]=-1; continue; }
        tot[i]=o.total; if(best<0||o.total<bt){ best=i; bt=o.total; } result_free(&o);
    }
    if(verbose) for(int i=0;i<ALG_COUNT;i++) print_run(i,r,n,head,lo,hi,dir,1);
    printf("\n%-8s %-14s %s\n","Algo","Total movement","Avg seek");
    for(int i=0;i<ALG_COUNT;i++) if(tot[i]>=0)
        printf("%-8s %-14ld %.2f%s\n",algorithm_name(i),tot[i],(double)tot[i]/n,i==best?"   <-- best":"");
}

static int read_list(int*buf,int max){
    char line[8192]; if(!fgets(line,sizeof line,stdin)) return 0;
    int n=0; char*s=line;
    while(*s&&n<max){ while(*s&&(isspace((unsigned char)*s)||*s==','))s++; if(!*s)break;
        char*e; long v=strtol(s,&e,10); if(e==s) return -1; buf[n++]=(int)v; s=e; }
    return n;
}

static int ask_int(const char*q,int def){
    char l[64]; printf("%s [%d]: ",q,def); if(!fgets(l,sizeof l,stdin)) return def;
    return (l[0]=='\n')?def:atoi(l);
}

static void usage(const char*p){
    printf("NEXUS - Disk Scheduling Analyzer (CLI)\n\n"
           "Usage:\n  %s                       interactive menu\n"
           "  %s --demo                 run textbook example\n"
           "  %s [-a ALG] [-h HEAD] [-l LO] [-u HI] [-d left|right] [-v] REQ...\n\n"
           "  ALG = fcfs|sstf|scan|cscan|look|clook|all (default all)\n"
           "  -v  show path and ASCII graph for every algorithm (with -a all)\n\n"
           "Example: %s -a scan -h 53 98 183 37 122 14 124 65 67\n",p,p,p,p);
}

static int alg_id(const char*s){
    static const char*k[]={"fcfs","sstf","scan","cscan","look","clook","all"};
    for(int i=0;i<7;i++) if(!strcasecmp(s,k[i])) return i;
    return -1;
}

#define MAXR 10000
int main(int argc,char**argv){
    static const int EX[]={98,183,37,122,14,124,65,67};
    if(argc==1){
        static int r[MAXR]; int n=8,head=53,lo=0,hi=199,dir=1,a;
        memcpy(r,EX,sizeof EX);
        for(;;){
            printf("\n==== NEXUS Disk Scheduling ====\nRequests: ");
            for(int i=0;i<n&&i<20;i++) printf("%d ",r[i]);
            if(n>20) printf("...");
            printf("\nHead=%d Range=[%d,%d] Dir=%s\n"
                   "1) Set requests  2) Set head/range/direction  3) Run one algorithm\n"
                   "4) Compare all   5) Compare all (verbose)     0) Exit\n> ",head,lo,hi,dir?"right":"left");
            char c[16]; if(!fgets(c,sizeof c,stdin)) break;
            switch(c[0]){
                case '1':{ static int t[MAXR]; printf("Enter requests (comma/space separated): ");
                    int k=read_list(t,MAXR);
                    if(k<=0) printf("Invalid input.\n"); else { memcpy(r,t,(size_t)k*sizeof(int)); n=k; } break; }
                case '2': head=ask_int("Head",head); lo=ask_int("Min cylinder",lo); hi=ask_int("Max cylinder",hi);
                    dir=ask_int("Direction (1=right,0=left)",dir)?1:0; break;
                case '3': printf("0 FCFS 1 SSTF 2 SCAN 3 C-SCAN 4 LOOK 5 C-LOOK\n"); a=ask_int("Algorithm",2);
                    print_run(a,r,n,head,lo,hi,dir,1); break;
                case '4': run_all(r,n,head,lo,hi,dir,0); break;
                case '5': run_all(r,n,head,lo,hi,dir,1); break;
                case '0': return 0;
                default: printf("Unknown option.\n");
            }
        }
        return 0;
    }
    if(!strcmp(argv[1],"--demo")){ run_all(EX,8,53,0,199,1,0); return 0; }
    if(!strcmp(argv[1],"--help")){ usage(argv[0]); return 0; }
    int head=53,lo=0,hi=199,dir=1,alg=6,verbose=0,n=0; int*r=malloc((size_t)argc*sizeof(int)); if(!r) return 1;
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-a")&&i+1<argc){ alg=alg_id(argv[++i]); if(alg<0){ usage(argv[0]); free(r); return 1; } }
        else if(!strcmp(argv[i],"-h")&&i+1<argc) head=atoi(argv[++i]);
        else if(!strcmp(argv[i],"-l")&&i+1<argc) lo=atoi(argv[++i]);
        else if(!strcmp(argv[i],"-u")&&i+1<argc) hi=atoi(argv[++i]);
        else if(!strcmp(argv[i],"-d")&&i+1<argc) dir=strcasecmp(argv[++i],"left")?1:0;
        else if(!strcmp(argv[i],"-v")) verbose=1;
        else { char*e; long v=strtol(argv[i],&e,10); if(*e||e==argv[i]){ usage(argv[0]); free(r); return 1; } r[n++]=(int)v; }
    }
    if(n==0){ fprintf(stderr,"No requests given.\n"); usage(argv[0]); free(r); return 1; }
    if(alg==6) run_all(r,n,head,lo,hi,dir,verbose); else print_run(alg,r,n,head,lo,hi,dir,1);
    free(r); return 0;
}
