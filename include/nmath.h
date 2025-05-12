#ifndef NTIPPER_MATH_H
#define NTIPPER_MATH_H

// TODO: @ntipper ^ Using glm for OpenGL stuff at the moment. Maybe replace with our own?

struct v2
{
    float x;
    float y;
};

struct v3
{
	float x;
	float y;
	float z;
};

struct v4
{
	float x;
	float y;
	float z;
	float w;
};

int power(int base, int exp);
v2 v2_add(v2& a, v2& b);
v2 v2_substract(v2& a, v2& b);
v2 operator+(v2& a, v2& b);
v2 operator-(v2& a, v2& b);
float v2_length(v2& a);
float v2_dot(v2& a, v2& b);
float v2_dot(v2& a, v2& b, float cos_theta);
float ncos(v2& a, v2& b);
v2 normalize(v2& v);

v3 v3_add(v3& a, v3& b);
v3 v3_subtract(v3& a, v3& b);
v3 operator+(v3& a, v3& b);
v3 operator-(v3& a, v3& b);
float v3_length(v3& a);
float v3_dot(v3& a, v3& b);
v3 normalize(v3& v);

v4 v4_add(v4& a, v4& b);
v4 v4_substract(v4& a, v4& b);
v4 operator+(v4& a, v4& b);
v4 operator-(v4& a, v4& b);
float v4_length(v4& a);
float v4_dot(v4& a, v4& b);
v4 normalize(v4& v);
#endif
