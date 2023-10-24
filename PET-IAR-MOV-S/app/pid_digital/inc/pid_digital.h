#include "pico/stdlib.h"
typedef enum{
    ENABLE, 
    DISABLE, 
}status_pid_t ; 

typedef struct{
    status_pid_t status ;
    float kp    ; 
    float ki ; 
    float kd ; 
    uint16_t min_pwmh  ; 
    uint16_t min_pwmah ;  
}cfg_pid; 


void setttings_pid(float proportional, float integral, float derivative) ; 
void run_pid() ; 
void angle_set_point(float *sp); 
void compute_pid(float sp, float period_sample) ; 
void set_minsh(uint16_t set_min) ; 
void set_minsa(uint16_t set_min) ; 
void get_pid(cfg_pid *pid) ;
