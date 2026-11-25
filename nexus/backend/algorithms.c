#include <stdlib.h>
#include <string.h>
#include "algorithms.h"

static const char *NAMES[ALG_COUNT] = {"FCFS","SSTF","SCAN","C-SCAN","LOOK","C-LOOK"};

const char *algorithm_name(int id){ return (id>=0 && id<ALG_COUNT) ? NAMES[id] : "UNKNOWN"; }

/* overflow-safe comparator */
static int cmp(const void*a,const void*b){
    int x=*(const int*)a, y=*(const int*)b;
    return (x>y)-(x<y);
}

static void zero(Result*o){ if(o) memset(o,0,sizeof *o); }

static int init(Result*o,int n,int head,int lo,int hi,const int*r){
    if(!o) return -1;
    zero(o);
    if(n<=0||!r||lo>=hi||head<lo||head>hi) return -1;
    for(int i=0;i<n;i++) if(r[i]<lo||r[i]>hi) return -2;
    o->n=n; int cap=n+4;                       /* n requests + max 2 boundary + 1 jump */
    o->path=calloc(cap,sizeof(int)); o->seek=calloc(cap,sizeof(int)); o->served=calloc(cap,sizeof(int));
    if(!o->path||!o->seek||!o->served){ result_free(o); return -3; }
    return 0;
}

static void push(Result*o,int*cur,int pos,int served){
    int d=abs(pos-*cur);
    o->path[o->steps]=pos; o->seek[o->steps]=d; o->served[o->steps]=served;
    o->total+=d; *cur=pos; o->steps++;
}

static void finish(Result*o){ o->avg=(double)o->total/o->n; }

/* sorted copy split into L (<head) and H (>=head), both ascending; free(*L) */
static int split(const int*r,int n,int head,int**L,int*nl,int**H,int*nh){
    int*s=malloc((size_t)n*sizeof(int)); if(!s) return -3;
    memcpy(s,r,(size_t)n*sizeof(int)); qsort(s,(size_t)n,sizeof(int),cmp);
    int k=0; while(k<n&&s[k]<head)k++;
    *L=s; *nl=k; *H=s+k; *nh=n-k; return 0;
}

int fcfs(const int*r,int n,int head,int lo,int hi,int dir,Result*o){
    (void)dir; int e=init(o,n,head,lo,hi,r); if(e) return e; int c=head;
    for(int i=0;i<n;i++) push(o,&c,r[i],1);
    finish(o); return 0;
}

int sstf(const int*r,int n,int head,int lo,int hi,int dir,Result*o){
    (void)dir; int e=init(o,n,head,lo,hi,r); if(e) return e;
    char*u=calloc((size_t)n,1); if(!u){ result_free(o); return -3; }
    int c=head;
    for(int k=0;k<n;k++){
        int b=-1;
        for(int i=0;i<n;i++)
            if(!u[i]&&(b<0||abs(r[i]-c)<abs(r[b]-c))) b=i;
        u[b]=1; push(o,&c,r[b],1);
    }
    free(u); finish(o); return 0;
}

/* mode: 0 SCAN, 1 C-SCAN, 2 LOOK, 3 C-LOOK */
static int sweep(const int*r,int n,int head,int lo,int hi,int dir,Result*o,int mode){
    int e=init(o,n,head,lo,hi,r); if(e) return e;
    int *L,*H,nl,nh;
    if(split(r,n,head,&L,&nl,&H,&nh)){ result_free(o); return -3; }
    int c=head;
    if(dir){
        for(int i=0;i<nh;i++) push(o,&c,H[i],1);
        if(mode==0){ if(c!=hi) push(o,&c,hi,0); for(int i=nl-1;i>=0;i--) push(o,&c,L[i],1); }
        else if(mode==1){ if(c!=hi) push(o,&c,hi,0);
            if(nl){ push(o,&c,lo,0); for(int i=0;i<nl;i++) push(o,&c,L[i],1); } }
        else if(mode==2){ for(int i=nl-1;i>=0;i--) push(o,&c,L[i],1); }
        else { for(int i=0;i<nl;i++) push(o,&c,L[i],1); }
    } else {
        for(int i=nl-1;i>=0;i--) push(o,&c,L[i],1);
        if(mode==0){ if(c!=lo) push(o,&c,lo,0); for(int i=0;i<nh;i++) push(o,&c,H[i],1); }
        else if(mode==1){ if(c!=lo) push(o,&c,lo,0);
            if(nh){ push(o,&c,hi,0); for(int i=nh-1;i>=0;i--) push(o,&c,H[i],1); } }
        else if(mode==2){ for(int i=0;i<nh;i++) push(o,&c,H[i],1); }
        else { for(int i=nh-1;i>=0;i--) push(o,&c,H[i],1); }
    }
    free(L); finish(o); return 0;
}

int scan (const int*r,int n,int h,int lo,int hi,int d,Result*o){return sweep(r,n,h,lo,hi,d,o,0);}
int cscan(const int*r,int n,int h,int lo,int hi,int d,Result*o){return sweep(r,n,h,lo,hi,d,o,1);}
int look (const int*r,int n,int h,int lo,int hi,int d,Result*o){return sweep(r,n,h,lo,hi,d,o,2);}
int clook(const int*r,int n,int h,int lo,int hi,int d,Result*o){return sweep(r,n,h,lo,hi,d,o,3);}

int run_algorithm(int id,const int*r,int n,int h,int lo,int hi,int d,Result*o){
    switch(id){
        case FCFS:  return fcfs (r,n,h,lo,hi,d,o);
        case SSTF:  return sstf (r,n,h,lo,hi,d,o);
        case SCAN:  return scan (r,n,h,lo,hi,d,o);
        case CSCAN: return cscan(r,n,h,lo,hi,d,o);
        case LOOK:  return look (r,n,h,lo,hi,d,o);
        case CLOOK: return clook(r,n,h,lo,hi,d,o);
    }
    if(o) zero(o);
    return -4;
}

void result_free(Result*o){
    if(!o) return;
    free(o->path); free(o->seek); free(o->served);
    memset(o,0,sizeof *o);
}
