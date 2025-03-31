#ifndef NTIPPER_MATH_H
#define NTIPPER_MATH_H

// TODO: @ntipper ^ Using glm for OpenGL stuff at the moment. Maybe replace with our own?

struct Vector2D
{
    float x;
    float y;
};

Vector2D Add(Vector2D& a, Vector2D& b);
Vector2D Substract(Vector2D& a, Vector2D& b);
Vector2D operator+(Vector2D& a, Vector2D& b);
Vector2D operator-(Vector2D& a, Vector2D& b);
float Length(Vector2D a);
float Dot_2D(Vector2D a, Vector2D b);
float Dot_2D(Vector2D a, Vector2D b, float cos_theta);
float ncos(Vector2D a, Vector2D b);

#endif
