#ifndef MATH_H
#define MATH_H

#define SCALE 100000

#define sin(angle_deg) sin_num(angle_deg) / SCALE
#define cos(angle_deg) cos_num(angle_deg) / SCALE
#define tan(angle_deg) sin_num(angle_deg) / cos_num(angle_deg)

int sin_num(int angle_deg);
int cos_num(int angle_deg);

#endif
