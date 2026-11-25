/* Self-test with known textbook results. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "algorithms.h"
char* nexus_run(int,const char*,int,int,int,int); void nexus_free(char*);
static int fails=0;
#define CHECK(c,msg) do{ if(!(c)){ printf("FAIL: %s\n",msg); fails++; } else printf("ok:   %s\n",msg);}while(0)
int main(void){
    int r[]={98,183,37,122,14,124,65,67}; long exp_r[]={640,236,331,382,299,322};
    for(int i=0;i<ALG_COUNT;i++){ Result o; int e=run_algorithm(i,r,8,53,0,199,1,&o);
        char m[64]; snprintf(m,64,"%s right total=%ld",algorithm_name(i),exp_r[i]);
        CHECK(!e&&o.total==exp_r[i],m); result_free(&o); }
    { Result o; run_algorithm(SCAN,r,8,53,0,199,0,&o); CHECK(o.total==(53-0)+(183-0),"SCAN left total=236"); result_free(&o); }
    { Result o; run_algorithm(LOOK,r,8,53,0,199,0,&o); CHECK(o.total==(53-14)+(183-14),"LOOK left total=208"); result_free(&o); }
    { Result o; int bad[]={500}; CHECK(run_algorithm(FCFS,bad,1,53,0,199,1,&o)==-2,"out-of-range request rejected"); }
    { Result o; CHECK(run_algorithm(FCFS,r,8,53,100,50,1,&o)==-1,"bad range rejected"); }
    { Result o; CHECK(run_algorithm(99,r,8,53,0,199,1,&o)==-4,"bad algorithm id rejected"); }
    { int d[]={50,50,50}; Result o; CHECK(run_algorithm(SSTF,d,3,50,0,100,1,&o)==0&&o.total==0,"duplicates at head cost 0"); result_free(&o); }
    { int one[]={0}; Result o; CHECK(run_algorithm(CSCAN,one,1,0,0,199,1,&o)==0&&o.total==398-199+0,"head at edge ok (C-SCAN)"); result_free(&o); }
    { int one[]={5}; Result o; CHECK(run_algorithm(CSCAN,one,1,5,5,5,1,&o)==-1,"min==max rejected"); }
    { char*j=nexus_run(0,"98,x",53,0,199,1); CHECK(strstr(j,"error")!=NULL,"JSON: invalid token -> error"); nexus_free(j); }
    { char*j=nexus_run(0,"",53,0,199,1); CHECK(strstr(j,"error")!=NULL,"JSON: empty -> error"); nexus_free(j); }
    { char*j=nexus_run(9,"1",53,0,199,1); CHECK(strstr(j,"error")!=NULL,"JSON: bad id -> error"); nexus_free(j); }
    { char*j=nexus_run(2,"98,183,37,122,14,124,65,67",53,0,199,1); CHECK(strstr(j,"\"total\":331")!=NULL,"JSON: SCAN total 331"); nexus_free(j); }
    printf("\n%s (%d failures)\n",fails?"TESTS FAILED":"ALL TESTS PASSED",fails); return fails?1:0;
}
