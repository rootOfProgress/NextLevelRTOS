#ifndef TIM2_5_H
#define TIM2_5_H

extern void init_timer(unsigned int ,unsigned int ,unsigned int ,unsigned int *);
extern void start(unsigned int);
extern void stop(unsigned int);
extern unsigned int read_counter(unsigned int);

#endif