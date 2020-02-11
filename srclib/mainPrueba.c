#include "../include/libPoolThreads.h"

int main(){
    pool_thread *pt;

    pt = pool_th_ini(NULL,5);
    if(pt == NULL){
        printf("Error");
    }
    
    sleep(3);
    
    pool_th_stop(pt);
    pool_th_wait(pt);
    pool_th_destroy(pt);
}
