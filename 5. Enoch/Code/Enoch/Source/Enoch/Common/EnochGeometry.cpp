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