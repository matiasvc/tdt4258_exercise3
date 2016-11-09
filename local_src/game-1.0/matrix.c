#include "matrix.h"
#include <stdio.h>

Vec2 new_vec(float x, float y)
{
	Vec2 v;
	v.x = fixedpt_rconst(x);
	v.y = fixedpt_rconst(y);
	return v;
}

Vec2 vec_scalar_mul(Vec2 v, fixedpt a)
{
	Vec2 b;
	b.x = fixedpt_mul(v.x, a);
	b.y = fixedpt_mul(v.y, a);
	return b;
}

Vec2 vec_vec_add(Vec2 v, Vec2 w)
{
	Vec2 b;
	b.x = v.x + w.x;
	b.y = v.y + w.y;
	return b;
}

Vec2 mat_vec_mul(Mat2x2 A, Vec2 v)
{
	Vec2 b;
	b.x = fixedpt_mul(v.x, A.m11) + fixedpt_mul(v.y, A.m12);
	b.y = fixedpt_mul(v.x, A.m21) + fixedpt_mul(v.y, A.m22);
	return b;
}

Mat2x2 new_mat(float m11, float m12, float m21, float m22)
{
	Mat2x2 M;
	M.m11 = fixedpt_rconst(m11);
	M.m12 = fixedpt_rconst(m12);
	M.m21 = fixedpt_rconst(m21);
	M.m22 = fixedpt_rconst(m22);
	return M;
}

Mat2x2 mat_mat_mul(Mat2x2 A, Mat2x2 B)
{
	Mat2x2 C;
	C.m11 = fixedpt_mul(A.m11, B.m11) + fixedpt_mul(A.m12, B.m21);
	C.m12 = fixedpt_mul(A.m11, B.m12) + fixedpt_mul(A.m12, B.m22);
	C.m21 = fixedpt_mul(A.m21, B.m11) + fixedpt_mul(A.m22, B.m21);
	C.m22 = fixedpt_mul(A.m21, B.m12) + fixedpt_mul(A.m22, B.m22);
	return C;
}

Mat2x2 get_rotation_matrix(fixedpt theta)
{
	fixedpt cosTheta = fixedpt_cos(theta);
	fixedpt sinTheta = fixedpt_sin(theta);
	Mat2x2 M;
	M.m11 = cosTheta; M.m12 = -sinTheta;
	M.m21 = sinTheta; M.m22 = cosTheta;
	return M;
}

void printVec2(Vec2 v)
{
	float x = fixedpt_tofloat(v.x);
	float y = fixedpt_tofloat(v.y);
	printf("Vector:\n|%.4f|\n|%.4f|\n", x, y);
}

void printMat2x2(Mat2x2 M)
{
	float m11 = fixedpt_tofloat(M.m11);
	float m12 = fixedpt_tofloat(M.m12);
	float m21 = fixedpt_tofloat(M.m21);
	float m22 = fixedpt_tofloat(M.m22);

	printf("Matrix:\n|%.4f\t%.4f|\n|%.4f\t%.4f|\n", m11, m12, m21, m22);
}
