#include "math.h"

float root(int, int);


#define STEPS 20
#define POWER 3
#define MAX_DELAY 400

float root(int input, int n)
{
  return powf((float) input, (float) 1./n);
}

// void regulate_log(unsigned int start_speed, unsigned int target_speed)
// {

//     float min = log(start_speed); /// log(2);
//     float max = log(target_speed); /// log(2);
//     float factor = target_speed / log(50);
//     float resolution = (max - min) / STEPS;
//     float speed;
//     float current_speed;
//     float sleep_each_step = (MAX_DELAY / 49);
//     unsigned int round = 1;


//     while ((speed = (log(round) * factor) + start_speed ) < target_speed)
//     {
//         min += resolution;
//         if ((speed - (unsigned int) speed) > 0.5)
//             current_speed = ceil(speed);
//         else
//             current_speed = ceil(speed);

//         round++;
//     }
// }

// void regulate_4thpower(unsigned int start_speed, unsigned int target_speed)
// {
//     float min = root(start_speed, POWER);
//     float max = root(target_speed, POWER);
//     float resolution = (max - min) / STEPS;
//     float speed;
//     float current_speed;
//     float sleep_each_step = (MAX_DELAY / STEPS);
//     unsigned int round = 0;


//     while ((speed = pow(min, POWER)) < target_speed)
//     {
//         min += resolution;
//         if ((speed - (unsigned int) speed) > 0.5)
//             current_speed = ceil(speed);
//         else
//             current_speed = ceil(speed);


//     }
// }

void regulate_linear(unsigned int start_speed, unsigned int target_speed)
{
    float resolution = (float) target_speed - (float) start_speed;
    float speed;
    float current_speed;
    float sleep_each_step = (MAX_DELAY / resolution);
    unsigned int round = 0;


    while (start_speed++ != target_speed)
    {
        current_speed = start_speed;

    }
}

int main(void)
{
    regulate_linear(18, 98);
    // regulate_4thpower(18, 98);
    // regulate_log(18, 98);
    return 0;
}