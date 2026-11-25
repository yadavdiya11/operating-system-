/* JSON API layer: used by the Python server (shared library), by WebAssembly,
 * and by the self-test. Validates input and returns a malloc'd JSON string. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include "algorithms.h"

#define MAX_REQ 100000

/* "98,183 37" -> ints. Returns count, or -1 on bad token / too many values. */
static int parse(const char*s,int**out){
    int cap=16,n=0; int*a=malloc((size_t)cap*sizeof(int)); if(!a) return -1;
    while(*s){
        while(*s&&(isspace((unsigned char)*s)||*s==','))s++;
        if(!*s)break;
        char*e; long v=strtol(s,&e,10);
        if(e==s||v<-1000000||v>1000000||(*e&&!isspace((unsigned char)*e)&&*e!=',')||n>=MAX_REQ){ free(a); return -1; }
        if(n==cap){ cap*=2; int*t=realloc(a,(size_t)cap*sizeof(int)); if(!t){ free(a); return -1; } a=t; }
        a[n++]=(int)v; s=e;
    }
    if(n==0){ free(a); a=NULL; }
    *out=a; return n;
}

static char* err(const char*t){
    char*m=malloc(160); if(!m) return NULL;
    snprintf(m,160,"{\"error\":\"%s\"}",t); return m;
}

EMSCRIPTEN_KEEPALIVE
char* nexus_run(int id,const char*reqs,int head,int lo,int hi,int dir){
    int*a=NULL; int n=parse(reqs?reqs:"",&a); Result r;
    if(id<0||id>=ALG_COUNT){ free(a); return err("Unknown algorithm"); }
    if(n<=0) return err("Request list is empty or contains an invalid value");
    int e=run_algorithm(id,a,n,head,lo,hi,dir?1:0,&r); free(a);
    if(e) return err(e==-2?"A request is outside the cylinder range":
                     e==-1?"Invalid cylinder range or head position":"Internal error");
    size_t sz=256+(size_t)r.steps*48, p=0; char*o=malloc(sz);
    if(!o){ result_free(&r); return err("Out of memory"); }
    p+=(size_t)snprintf(o+p,sz-p,
        "{\"name\":\"%s\",\"id\":%d,\"head\":%d,\"min\":%d,\"max\":%d,\"total\":%ld,\"avg\":%.4f,\"n\":%d,\"steps\":[",
        algorithm_name(id),id,head,lo,hi,r.total,r.avg,r.n);
    for(int i=0;i<r.steps;i++)
        p+=(size_t)snprintf(o+p,sz-p,"%s{\"p\":%d,\"s\":%d,\"v\":%d}",i?",":"",r.path[i],r.seek[i],r.served[i]);
    snprintf(o+p,sz-p,"]}");
    result_free(&r); return o;
}

EMSCRIPTEN_KEEPALIVE void nexus_free(char*p){ free(p); }
