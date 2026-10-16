#include <math.h>
#include <stddef.h>

#define PI       3.14159265358979323846
#define HALF_PI  1.57079632679489661923
#define QUART_PI 0.78539816339744830962
#define TWO_PI   6.28318530717958647692

#define LN2      0.69314718055994530942

// A precomputed lookup table holding sin(0) to sin(90) scaled up by 100,000.
// For example: sin(30) = 0.5, stored here as 50000.
static const unsigned int sin_table[] = {
    0,     1745,  3490,  5234,  6976,  8716,  10453, 12187, 13917, 15643, 17365,
    19081, 20791, 22495, 24192, 25882, 27564, 29237, 30902, 32557, 34202, 35837,
    37461, 39073, 40674, 42262, 43837, 45399, 46947, 48481, 50000, 51504, 52992,
    54464, 55919, 57358, 58779, 60182, 61566, 62932, 64279, 65606, 66913, 68200,
    69466, 70711, 71934, 73135, 74314, 75471, 76604, 77715, 78801, 79864, 80902,
    81915, 82904, 83867, 84805, 85717, 86603, 87462, 88295, 89101, 89879, 90631,
    91355, 92050, 92718, 93358, 93969, 94552, 95106, 95630, 96126, 96593, 97030,
    97437, 97815, 98163, 98481, 98769, 99027, 99255, 99452, 99619, 99756, 99863,
    99939, 99985, 100000
};

// Finds square root using the "averaging guess" method (Newton's method).
// If your guess for sqrt(x) is 'val', then x / val is another guess.
// One guess is always too big and the other too small, so taking their
// average gives a much better guess. Repeat 12 times for full precision.
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

// Calculates sin(x) in radians.
// Step 1: Wrap huge angles down to one full circle (-2pi to +2pi).
// Step 2: Use symmetry to fold any angle into the target quarter-circle (-pi/2 to +pi/2).
// Step 3: Run the polynomial wave formula x - x^3/3! + x^5/5! - x^7/7!...
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

// Cosine is just sine shifted by 90 degrees (pi/2 radians).
double cos(double x) {
    return sin(x + HALF_PI);
}

// Tangent is the ratio of opposite over adjacent, which equals sin(x) / cos(x).
double tan(double x) {
    return sin(x) / cos(x);
}

// Evaluates arctan(x) using the formula: x - x^3/3 + x^5/5 - x^7/7 + ...
// Only works fast and accurate when x is a small fraction close to zero.
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

// Calculates arctan(x) (finds the angle whose tangent is x).
// For negative x: uses symmetry arctan(-x) = -arctan(x).
// For x > 1: flips input (1/x) using the identity arctan(x) = pi/2 - arctan(1/x).
// For medium x: shrinks input using arctan(x) = pi/4 + arctan((x-1)/(x+1)).
// Once x is tiny, passes it to atan_eval().
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

// Calculates arctan(y / x) for any 2D point (x, y).
// Standard atan(y/x) gets confused about which side of the graph you are on.
// This function looks at the signs (+ or -) of x and y to figure out the exact
// angle in all 4 quadrants (from -180 to +180 degrees).
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

// Calculates arcsin(x) (finds the angle whose sine is x).
// Imagine a right triangle with hypotenuse 1 and opposite side x.
// By Pythagoras, the adjacent side is sqrt(1 - x^2).
// Since tan(angle) = opposite / adjacent, we get: angle = atan2(x, sqrt(1 - x^2)).
double asin(double x) {
    if (x >= 1.0) {
        return HALF_PI;
    }
    if (x <= -1.0) {
        return -HALF_PI;
    }
    return atan2(x, sqrt(1.0 - x * x));
}

// Arccos(x) is the angle whose cosine is x.
// Sine and cosine are complementary angles, so arccos(x) = 90 deg - arcsin(x).
double acos(double x) {
    return HALF_PI - asin(x);
}

// Fast integer sine lookup function using integer degrees (0 to 360).
// Step 1: Wrap negative or huge angles into standard [0, 359] range.
// Step 2: Use symmetry to map all 4 quadrants into standard 0 to 90 degree lookups.
// Returns integer values scaled up by 100,000 (e.g., 100000 = 1.0, 50000 = 0.5).
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

// Fast integer cosine lookup.
// Cosine is just sine shifted backward by 90 degrees.
int cos_num(int angle_deg) { 
    return sin_num(90 - angle_deg); 
}

// Absolute value: removes the negative sign if present.
double fabs(double x) {
    return x < 0.0 ? -x : x;
}

// Floor: rounds down to the nearest whole integer (e.g. 3.7 -> 3.0, -2.1 -> -3.0).
double floor(double x) {
    long i = (long)x;
    double d = (double)i;
    if (x < d) {
        return d - 1.0;
    }
    return d;
}

// Ceiling: rounds up to the nearest whole integer (e.g. 3.1 -> 4.0, -2.9 -> -2.0).
double ceil(double x) {
    long i = (long)x;
    double d = (double)i;
    if (x > d) {
        return d + 1.0;
    }
    return d;
}

// Floating-point remainder after dividing x by y (like the % operator for integers).
double fmod(double x, double y) {
    if (y == 0.0) {
        return 0.0;
    }
    double quotient = x / y;
    long i = (long)quotient;
    return x - ((double)i * y);
}

// Computing e^x directly with Taylor series breaks down for large x.
// To fix this, we pull out powers of 2 by expressing x as:
//   x = k * ln(2) + r
//
// Raising e to both sides:
//   e^x = e^(k * ln(2) + r)
//       = e^(k * ln(2)) * e^r
//       = (e^(ln(2)))^k * e^r
//       = 2^k * e^r
//
// Because k * ln(2) absorbs most of x, the remainder r stays bounded within
// [-ln(2)/2, ln(2)/2] (about -0.34657 to 0.34657).
// A tiny remainder r lets e^r converge in just 12 Taylor steps, then we scale by 2^k.
double exp(double x) {
    if (x == 0.0) {
        return 1.0;
    }

    int k = (int)(x / LN2 + (x < 0.0 ? -0.5 : 0.5));
    double r = x - (double)k * LN2;

    double term = 1.0;
    double sum = 1.0;
    for (int i = 1; i <= 12; i++) {
        term *= r / (double)i;
        sum += term;
    }

    double scale = 1.0;
    if (k > 0) {
        while (k--) scale *= 2.0;
    } else if (k < 0) {
        while (k++) scale /= 2.0;
    }

    return sum * scale;
}

// First, split x into mantissa and exponent: x = m * 2^k  (where 1.0 <= m < 2.0).
// Taking log: ln(x) = ln(m * 2^k) = ln(m) + k * ln(2).
//
// Standard ln(1+x) series is too slow for m near 2.
// Switching variables to z = (m - 1) / (m + 1) gives the identity:
//   ln(m) = ln((1 + z) / (1 - z)) = 2 * (z + z^3/3 + z^5/5 + ...)
//
// Since m is in [1, 2), z stays small (z < 1/3), so this series hits full
// double precision in just 7 terms.
double log(double x) {
    if (x <= 0.0) {
        return 0.0;
    }

    int k = 0;
    while (x >= 2.0) {
        x /= 2.0;
        k++;
    }
    while (x < 1.0) {
        x *= 2.0;
        k--;
    }

    double z = (x - 1.0) / (x + 1.0);
    double z2 = z * z;
    double term = z;
    double sum = z;

    for (int i = 3; i <= 15; i += 2) {
        term *= z2;
        sum += term / (double)i;
    }

    return 2.0 * sum + (double)k * LN2;
}

// Uses the algebraic rule: b^y = (e^ln(b))^y = e^(y * ln(b)).
//
// Since ln(b) is undefined for negative numbers, for b < 0 we compute
// e^(y * ln(|b|)) instead, then check if y is an integer to pick the sign
// (odd integer = negative, even integer = positive, fraction = invalid).
double pow(double base, double exponent) {
    if (exponent == 0.0) {
        return 1.0;
    }
    if (base == 0.0) {
        return 0.0;
    }
    if (base < 0.0) {
        long iexp = (long)exponent;
        if ((double)iexp == exponent) {
            double res = exp(exponent * log(-base));
            return (iexp % 2 != 0) ? -res : res;
        }
        return 0.0;
    }

    return exp(exponent * log(base));
}
