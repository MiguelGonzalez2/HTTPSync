#include "libPoolThreads.h"
#include <sys/signal.h>
#include <unistd.h>

void work(){
   printf("HILO TRABAJANDO!\n");
   sleep(2);
}

int main(){
    pool_thread *pt;

    pt = pool_th_ini(&work,5);
    if(pt == NULL){
        printf("Error");
    }
    
    sleep(10);
    
    pool_th_stop(pt);
    pool_th_wait(pt);
    pool_th_destroy(pt);
}
