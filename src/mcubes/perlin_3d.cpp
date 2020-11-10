// The 2D version of ths was on Wikipedia. Adapted for 3D Perlin noise.

#include <math.h>

/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
float interpolate(float a0, float a1, float w) {
     if (0.0 > w) return a0;
     if (1.0 < w) return a1;
     return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

typedef struct {
    float x, y, z;
} vector3;

/* Create random direction vector
 */
vector3 randomGradient(int ix, int iy, int iz) {
    // Random float. No precomputed gradients mean this works for any number of grid coordinates
    float random = 2920.f * sin(ix * 21942.f + iy * 171324.f + 8912.f) * cos(ix * 23157.f * iy * 217832.f + 9758.f) * tan(iz);
    return (vector3) { .x = cos(random), .y = sin(random), .z = acos(random)};
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, int iz, float x, float y, float z) {
    // Get gradient from integer coordinates
    vector3 gradient = randomGradient(ix, iy, iz);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;
    float dz = z - (float)iz;

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y + dz*gradient.z);
}

// Compute Perlin noise at coordinates x, y
float perlin(float x, float y, float z) {
    // Determine grid cell coordinates
    int x0 = (int)x;
    int x1 = x0 + 1;
    int y0 = (int)y;
    int y1 = y0 + 1;
    int z0 = (int)z;
    int z1 = z0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;
    float sz = z - (float)z0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, z0, x, y, z);
    n1 = dotGridGradient(x1, y0, z0, x, y, z);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, z1, x, y, z);
    n1 = dotGridGradient(x1, y1, z1, x, y, z);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value;
}

