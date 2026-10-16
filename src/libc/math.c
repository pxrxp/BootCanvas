#include <math.h>
#define PI       3.14159265358979323846
#define HALF_PI  1.57079632679489661923
#define QUART_PI 0.78539816339744830962
#define TWO_PI   6.28318530717958647692

static const unsigned int sin_table[] = {
    0,     1745,  3490,  5234,  6976,  8716,  10453, 12187, 13917, 15643, 17365,
    19081, 20791, 22495, 24192, 25882, 27564, 29237, 30902, 32557, 34202, 35837,
    37461, 39073, 40674, 42262, 43837, 45399, 46947, 48481, 50000, 51504, 52992,
    54464, 55919, 57358, 58779, 60182, 61566, 62932, 64279, 65606, 66913, 68200,
    69466, 70711, 71934, 73135, 74314, 75471, 76604, 77715, 78801, 79864, 80902,
    81915, 82904, 83867, 84805, 85717, 86603, 87462, 88295, 89101, 89879, 90631,
    91355, 92050, 92718, 93358, 93969, 94552, 95106, 95630, 96126, 96593, 97030,
    97437, 97815, 98163, 98481, 98769, 99027, 99255, 99452, 99619, 99756, 99863,
    99939, 99985, 100000};

double sqrt(double x) {
    if (x <= 0.0) {
        return 0.0;
    }
    double val = (x < 1.0) ? 1.0 : x;
    for (int i = 0; i < 12; i++) {
        val = 0.5 * (val + x / val);
    }
    return val;
}

double sin(double x) {
    long i = (long)(x / TWO_PI);
    x -= (double)i * TWO_PI;

    if (x > PI) {
        x -= TWO_PI;
    } else if (x < -PI) {
        x += TWO_PI;
    }

    if (x > HALF_PI) {
        x = PI - x;
    } else if (x < -HALF_PI) {
        x = -PI - x;
    }

    double x2 = x * x;
    return x * (1.0 - x2 * (1.0 / 6.0 - x2 * (1.0 / 120.0 - x2 * (1.0 / 5040.0 - x2 * (1.0 / 362880.0 - x2 * (1.0 / 39916800.0 - x2 * (1.0 / 6227020800.0)))))));
}

double cos(double x) {
    return sin(x + HALF_PI);
}

double tan(double x) {
    return sin(x) / cos(x);
}

static double atan_eval(double x) {
    double x2 = x * x;
    double term = x;
    double res = term;

    for (int i = 3; i <= 25; i += 2) {
        term = -term * x2;
        res += term / (double)i;
    }

    return res;
}

double atan(double x) {
    if (x < 0.0) {
        return -atan(-x);
    }
    if (x > 1.0) {
        return HALF_PI - atan(1.0 / x);
    }
    if (x > 0.41421356237309504880) {
        return QUART_PI + atan((x - 1.0) / (x + 1.0));
    }
    return atan_eval(x);
}

double atan2(double y, double x) {
    if (x > 0.0) {
        return atan(y / x);
    }
    if (x < 0.0) {
        if (y >= 0.0) {
            return atan(y / x) + PI;
        }
        return atan(y / x) - PI;
    }
    if (y > 0.0) {
        return HALF_PI;
    }
    if (y < 0.0) {
        return -HALF_PI;
    }
    return 0.0;
}

double asin(double x) {
    if (x >= 1.0) {
        return HALF_PI;
    }
    if (x <= -1.0) {
        return -HALF_PI;
    }
    return atan2(x, sqrt(1.0 - x * x));
}

double acos(double x) {
    return HALF_PI - asin(x);
}

int sin_num(int angle_deg) {
    angle_deg %= 360;
    if (angle_deg < 0) {
        angle_deg += 360;
    }

    if (angle_deg <= 90) {
        return sin_table[angle_deg];
    }
    if (angle_deg <= 180) {
        return sin_table[180 - angle_deg];
    }
    if (angle_deg <= 270) {
        return -sin_table[angle_deg - 180];
    }
    return -sin_table[360 - angle_deg];
}

int cos_num(int angle_deg) { 
    return sin_num(90 - angle_deg); 
}

double fabs(double x) {
    return x < 0.0 ? -x : x;
}

double floor(double x) {
    long i = (long)x;
    double d = (double)i;
    if (x < d) {
        return d - 1.0;
    }
    return d;
}

double ceil(double x) {
    long i = (long)x;
    double d = (double)i;
    if (x > d) {
        return d + 1.0;
    }
    return d;
}

double fmod(double x, double y) {
    if (y == 0.0) {
        return 0.0;
    }
    double quotient = x / y;
    long i = (long)quotient;
    return x - ((double)i * y);
}
