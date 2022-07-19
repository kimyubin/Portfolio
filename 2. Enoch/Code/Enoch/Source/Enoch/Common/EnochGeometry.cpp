#include "EnochGeometry.h"

VectorF2D::VectorF2D(Vector2D pos)
{
    *this = VectorF2D(pos.x,pos.y);
} 

VectorF2D VectorF2D::Normalize(float targetLength=1)
{
    auto length = sqrt(x*x+y*y);
    if(length == 0) return VectorF2D(0,0);
    targetLength /= length;
    return VectorF2D(x*targetLength, y*targetLength);
}

inline VectorF2D Vector2D::operator + (VectorF2D rhs) { return VectorF2D(x + rhs.x, y + rhs.y); }
inline VectorF2D Vector2D::operator - (VectorF2D rhs) { return VectorF2D(x - rhs.x, y - rhs.y); }
inline VectorF2D Vector2D::operator * (float rhs) { return VectorF2D(x * rhs, y * rhs); }