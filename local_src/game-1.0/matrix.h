#ifndef __INCLUDE_MATRIX_H__
#define __INCLUDE_MATRIX_H__

#include "fixedptc.h"

typedef struct 
{
	fixedpt x;
	fixedpt y;
} Vec2;

// Defines a 2x2 matrix with elements:
// | m11 m12 |
// | m21 m22 |
typedef struct
{
	fixedpt m11;
	fixedpt m12;
	fixedpt m21;
	fixedpt m22;
} Mat2x2;

Vec2 new_vec(float x, float y);
Vec2 get_rot_vec(fixedpt theta);
Vec2 normalize_vec(Vec2 v);
Vec2 vec_scalar_mul(Vec2 v, fixedpt a);
Vec2 vec_vec_add(Vec2 v, Vec2 w);
Vec2 mat_vec_mul(Mat2x2 A, Vec2 v);
fixedpt vec_length(Vec2 v);
Mat2x2 new_mat(float m11, float m12, float m21, float m22);
Mat2x2 mat_mat_mul(Mat2x2 A, Mat2x2 B);
Mat2x2 get_rotation_matrix(fixedpt theta);

void printVec2(Vec2 v);
void printMat2x2(Mat2x2 M);

#endif // #ifndef __INCLUDE_MATRIX_H__
