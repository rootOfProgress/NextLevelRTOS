#ifndef RPM_H
#define RPM_H

typedef struct rpm {
    unsigned int engine_0;
    unsigned int engine_1;
    unsigned int engine_2;
    unsigned int engine_3;
} Rpm_t;

extern Rpm_t rpm_results;

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) init_rpm_timer(void);
void __attribute__((__noipa__))  __attribute__((optimize("O0"))) rpm_main(void);
void __attribute__((__noipa__))  __attribute__((optimize("O0"))) do_measurement();

#endif