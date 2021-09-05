#pragma once

#include<math.h>

//필드위 좌표. 칸을 기준으로 나눔
struct Vector2D {
    int y;
    int x;
	inline bool operator == (Vector2D rhs) { return (y == rhs.y) && (x == rhs.x);}
	inline Vector2D operator + (Vector2D rhs) {	return {x + rhs.x, y + rhs.y};}
	inline Vector2D operator - (Vector2D rhs) { return {x - rhs.x, y - rhs.y}; }
};

//실수 좌표계. 투사체에 주로 사용
struct VectorF2D {
	float y;
	float x;
    VectorF2D() : x(0), y(0) {};
    VectorF2D(int xIn, int yIn) : x(xIn), y(yIn) {};
    VectorF2D(float xIn, float yIn) : x(xIn), y(yIn) {};
    VectorF2D(Vector2D pos);
	inline bool operator == (VectorF2D rhs) { return (y == rhs.y) && (x == rhs.x);}
	inline VectorF2D operator + (VectorF2D rhs) {	return VectorF2D(x + rhs.x, y + rhs.y);}
	inline VectorF2D operator - (VectorF2D rhs) { return VectorF2D(x - rhs.x, y - rhs.y); }
    Vector2D GetFieldPoint() { return {int(y+0.5f), int(x+0.5f)}; }   //반올림
    VectorF2D Normalize(float targetLength);
};