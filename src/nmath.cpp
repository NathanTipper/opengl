#include "nmath.h"
#include <cmath>

#define FLOAT_APPROX_ZERO 0.00000001f
int power(int base, int power)
{
    if(power == 0)
    {
	    return 1;
    }

    if(power < 0)
    {
	    return -1;
    }

    int Result = base;

    for(int i = 0; i < power - 1; ++i)
    {
	    Result *= base;
    }

    return Result;
}

v2 v2_add(v2& a, v2& b)
{
    v2 Result;
    Result.x = a.x + b.x;
    Result.y = a.y + b.y;

    return(Result);
}

v2 v2_substract(v2& a, v2& b)
{
    v2 Result;
    Result.x = a.x - b.x;
    Result.y = a.y - b.y;

    return(Result);
}

v2 operator+(v2& a, v2& b)
{
    return v2_add(a, b);
}

v2 operator-(v2& a, v2& b)
{
    return v2_substract(a, b);
}

float v2_length(v2& a)
{
    return(std::sqrt((a.x * a.x) + (a.y * a.y)));
}

float v2_dot(v2& a, v2& b)
{
    return((a.x * b.x) + (a.y * b.y));
}

float v2_dot(v2& a, v2& b, float cos_theta)
{
    float la = v2_length(a);
    float lb = v2_length(b);

    return la * lb * cos_theta;
}

float ncos(v2& a, v2& b)
{
    float Result;
    float lengthA = v2_length(a);
    float lengthB = v2_length(b);
    float DotAB = v2_dot(a, b);
    if(lengthA * lengthB < FLOAT_APPROX_ZERO)
    {
        return 0.f;
    }

    Result = DotAB / (lengthA * lengthB);

    return Result;
}

v2 v2_normalize(v2& v)
{
    float l = v2_length(v);

    if(l < FLOAT_APPROX_ZERO)
    {
        return v;
    }

    return { v.x / l, v.y / l};
}


v3 v3_add(v3& a, v3& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

v3 v3_subtract(v3& a, v3& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

v3 operator+(v3& a, v3& b)
{
    return v3_add(a, b);
}

v3 operator-(v3& a, v3& b)
{
    return v3_subtract(a, b);
}

float v3_length(v3& a)
{
   float x_sq = a.x * a.x;
   float y_sq = a.y * a.y;
   float z_sq = a.z * a.z;

   return sqrt(x_sq + y_sq + z_sq);
}

float v3_dot(v3& a, v3& b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

v3 v3_normalize(v3& v)
{
    float l = v3_length(v);

    if(l < FLOAT_APPROX_ZERO)
    {
        return v;
    }

    return { v.x / l, v.y / l, v.z / l };
}

v4 v4_add(v4& a, v4& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

v4 v4_substract(v4& a, v4& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

v4 operator+(v4& a, v4& b)
{
    return v4_add(a, b);
}

v4 operator-(v4& a, v4& b)
{
    return v4_substract(a, b);
}

float v4_length(v4& a)
{
   float x_sq = a.x * a.x;
   float y_sq = a.y * a.y;
   float z_sq = a.z * a.z;
   float w_sq = a.w * a.w;

   return sqrt(x_sq + y_sq + z_sq + w_sq);
}

float v4_dot(v4& a, v4& b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
