#ifndef MATH_H
#define MATH_H

#define SCALE 100000

// x * SIN(30) is valid, SIN(30) * x is invalid. Must be left-multiplied.
#define SIN(angle_deg) sin_num(angle_deg) / SCALE
#define COS(angle_deg) cos_num(angle_deg) / SCALE
#define TAN(angle_deg) sin_num(angle_deg) / cos_num(angle_deg)

int sin_num(int angle_deg);
int cos_num(int angle_deg);

#endif
