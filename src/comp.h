#ifndef COMP_H
#define COMP_H

#define DGL_version "2024.4.1 (Linux)"

#define colour_compare(x, y) (((x).r == (y).r) && ((x).g == (y).g) && ((x).b == (y).b) && ((x).a == (y).a))

#define toggle int
extern int windowWidth, windowHeight;

#endif