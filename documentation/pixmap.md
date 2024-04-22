The following is documentation for the function `pixmap_generate` and related functions and data structures.

The function takes the form

```c
Color *pixmap_generate(int shape_type, union shape_info dat, Color colour, Color *out)
```

### *shape_type* and *dat*

The enum `shape_types` has the definitions for shapes supported by `pixmap_generate`.

`dat` holds information about the shape. It is a union of a 2-element vector (v2), a 3-element vector (v3) and a 4-element vector (v4). Use the vector with the appropriate number of elements according to the requirements for `pixmap_generate`, as described below. For example, is a shape has width, height, and density, use v3 to pass all three elements to `pixmap_generate`.

All shapes use *x* for width and *y* for height. *Extra* dat *parameters* specifies use for *w* and *z*

|numeric value | enum *shape_types* definition | description | extra *dat* parameters|
|---|---|---|---|
|0|NOTHING|none|none|
|1|RECT|Rectangle or square|none|
|2|CIRCLE| Circle|none|
|3|ELLIPSE| Ellipse (slower than CIRCLE)|none|
|4|EQUILATERAL| Equilateral triangle|none|
|5|SC_RECT|Special Case Rectangle| z: horizontal confinement (closer to the origin)|
|6|SC_CIRCLE| Special Case Circle|z: amount to decrease radius by|
|7|SCATTER| Randomly scattered points (1x1 px)|z: density (1 in z)|
|8| SC_SCATTER| (Special Case) Randomly scattered points| w: size of points, z: density (1 in z)|

### *out*

Pointer to already allocated memory. It is assumed this memory is large enough to hold the entire pixmap. If the memory already contained a pixmap, the pre-existing pixmap will *not* be cleared, but rather the new shape will be drawn on top of the existing image.

### shinf

DGL provides the *shinf* macros to simplify the proess of passing a `union shape_info` to `pixmap_generate`.

- `shinf(_x, _y)` passes the *x* and *y* values to `pixmap_generate` through v2.
- `shinf3(_x, _y, _z)` passes the *x*, *y*, and *z* values to `pixmap_generate` through v3.
- `shinf4(_w, _x, _y _z)` passes the *w*, *x*, *y*, and *z* values to `pixmap_generate` through v4.  
