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

/*
	Returns a new vector.
	@param x	the x value of the vector
	@param y	the y value of the vector
*/
Vec2 new_vec(float x, float y);

/*
	Returns rotated unit vector.
	@param theta	the angle of the vector in radians
*/
Vec2 get_rot_vec(fixedpt theta);

/*
	Returns the normalized vector.
	@param v	the vector to normalize
*/
Vec2 normalize_vec(Vec2 v);

/*
	Multiplies a vector with a scalar.
	@param v	the vector
	@param a	the scalar
*/
Vec2 vec_scalar_mul(Vec2 v, fixedpt a);

/*
	Adds to vectors.
	@param v	one vector
	@param w	the other vector
*/
Vec2 vec_vec_add(Vec2 v, Vec2 w);

/*
	Right multiplies a matrix with a vector.
	@param A	the matrix
	@param v	the vector
*/
Vec2 mat_vec_mul(Mat2x2 A, Vec2 v);

/*
	Returns the length of a vector.
	@param v	the vector
*/
fixedpt vec_length(Vec2 v);

/*
	Returns a new matrix.
	@param m11	row 1 column 1 value
	@param m12	row 1 column 2 value
	@param m21	row 2 column 1 value
	@param m22	row 2 column 2 value
*/
Mat2x2 new_mat(float m11, float m12, float m21, float m22);

/*
	Multiplies two matrices.
	@param A	the left matrix
	@param B	the right matrix
*/
Mat2x2 mat_mat_mul(Mat2x2 A, Mat2x2 B);

/*
	Returns a 2D rotation matrix.
	@param theta	angle of rotation in radians
*/
Mat2x2 get_rotation_matrix(fixedpt theta);

/*
	Prints a vector.
	@param v	the vector to print
*/
void printVec2(Vec2 v);

/*
	Prints a matrix.
	@param M	the matrix to print
*/
void printMat2x2(Mat2x2 M);

#endif // #ifndef __INCLUDE_MATRIX_H__
