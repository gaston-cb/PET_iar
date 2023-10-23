#include "pico/stdlib.h"




void setttings_pid(float proportional, float integral, float derivative) ; 
void compute_pid(float sp, float period_sample) ; 
void set_minsh(uint16_t set_min) ; 
void set_minsa(uint16_t set_min) ; 