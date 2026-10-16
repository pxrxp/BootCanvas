#ifndef MATH_H
#define MATH_H

#define SCALE 100000
#define PI       3.14159265358979323846
#define HALF_PI  1.57079632679489661923
#define QUART_PI 0.78539816339744830962
#define TWO_PI   6.28318530717958647692

// x * SIN(angle) is valid
// SIN(angle) * x is not
#define SIN(angle_deg) sin_num(angle_deg) / SCALE
#define COS(angle_deg) cos_num(angle_deg) / SCALE
#define TAN(angle_deg) sin_num(angle_deg) / cos_num(angle_deg)

int sin_num(int angle_deg);
int cos_num(int angle_deg);

double sqrt(double x);
double sin(double x);
double cos(double x);
double tan(double x);
double atan(double x);
double atan2(double y, double x);
double asin(double x);
double acos(double x);

#endif
