#ifndef DGL_MATH_H
#define DGL_MATH_H


#define squared(x) ((x) * (x))
#define distance(x1, y1, x2, y2) sqrt(squared((x2) - (x1)) + squared((y2) - (y1)))

#define larger(x, y) (((x) >= (y)) ? (x) : (y))
#define smaller(x, y) (((x) >= (y)) ? (y) : (x))

#define slope_intercept_x(y, m, b) (((y - b) / m) + (dat.v2.x / 2))
#define slope_intercept_y(x, m, b) (m * (x) + b)

#endif