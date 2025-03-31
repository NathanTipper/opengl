#include "nmath.h"
#include <cmath>

Vector2D Add(Vector2D& a, Vector2D& b)
{
    Vector2D Result;
    Result.x = a.x + b.x;
    Result.y = a.y + b.y;

    return(Result);
}

Vector2D Substract(Vector2D& a, Vector2D& b)
{
    Vector2D Result;
    Result.x = a.x - b.x;
    Result.y = a.y - b.y;

    return(Result);
}

Vector2D operator+(Vector2D& a, Vector2D& b)
{
    return Add(a, b);
}

Vector2D operator-(Vector2D& a, Vector2D& b)
{
    return Substract(a, b);
}

float Length(Vector2D a)
{
    return(std::sqrt((a.x * a.x) + (a.y * a.y)));
}

float Dot_2D(Vector2D a, Vector2D b)
{
    return((a.x * b.x) + (a.y * b.y));
}

float Dot_2D(Vector2D a, Vector2D b, float cos_theta)
{
    return -1.0f;
}

float ncos(Vector2D a, Vector2D b)
{
    float Result;
    float lengthA = Length(a);
    float lengthB = Length(b);
    float DotAB = Dot_2D(a, b);
    if(lengthA * lengthB == 0.f)
    {
        return 0.f;
    }
    Result = DotAB / (lengthA * lengthB);

    return Result;
}
