#include "vecmath.h"
#include <stdio.h>

template <size_t WIDTH, size_t HEIGHT>
using Mtx = BaseMtx<float, WIDTH, HEIGHT>;

template <size_t SIZE>
using MtxSq = BaseMtxSq<float, SIZE>;

template <size_t SIZE>
using Vec = BaseVec<float, SIZE>;

// The degree of the curve to fit the data points to
// 0 = constant, 1 = linear, 2 = quadratic, 3 = cubic, etc.
#define DEGREE() 1

struct ConstraintPoint
{
	float x = 0.0f;
	float y = 0.0f;
};

struct DataPoint
{
	float x = 0.0f;
	float y = 0.0f;
	float weight = 1.0f;
};

int main(int argc, char** argv)
{
	DataPoint dataPoints[] =
	{
		{1.0f, 2.0f, 1.0f},
		{2.0f, 4.0f, 100.0f},
		{3.0f, 3.0f, 1.0f},
	};

#if 1
	ConstraintPoint* constraintPoints = nullptr;
	static const int c_numConstraintPoints = 0;
#else
	ConstraintPoint constraintPoints[] =
	{
		{}
	};

	static const int c_numConstraintPoints = _countof(constraintPoints);
#endif

	static const int c_numPoints = _countof(dataPoints);

	// Create the W weight matrix, that has weight_i at W_ii
	MtxSq<c_numPoints> W{};
	for (int i = 0; i < c_numPoints; ++i)
		W[i][i] = dataPoints[i].weight;

	// Create the A matrix, where each row is a power of x, x^n where N is from 0 up to and including DEGREE()
	// Also the A^T transpose.
	Mtx<DEGREE() + 1, c_numPoints> A;
	for (int y = 0; y < c_numPoints; ++y)
		for (int x = 0; x < DEGREE() + 1; ++x)
			A[y][x] = powf(dataPoints[y].x, float(x));
	auto AT = Transpose(A);

	// Make the Y vector, which is just the y values of each point
	Vec<c_numPoints> Y;
	for (int i = 0; i < c_numPoints; ++i)
		Y[i] = dataPoints[i].y;

	// Calculate A^T * W * A
	// Also calculate A^T * W * Y
	auto ATW = Multiply(AT, W);
	auto ATWA = Multiply(ATW, A);
	auto ATWY = Multiply(ATW, Y);

	// Make an augmented matrix where the matrix A^T*W*A is on the left side, and the vector A^T*W*Y is on the right side.
	Mtx<Columns(ATWA) + 1, DEGREE() + 1> augmentedMatrix;
	for (size_t iy = 0; iy < Rows(ATWA); ++iy)
	{
		for (size_t ix = 0; ix < Columns(ATWA); ++ix)
			augmentedMatrix[iy][ix] = ATWA[iy][ix];

		augmentedMatrix[iy][Columns(ATWA)] = ATWY[iy];
	}

	// Solve the equation for x:
	// A^TWA * x = A^TWY
	// x is the coefficients of the polynomial of our fit
	GaussJordanElimination(augmentedMatrix);

	// Get our coefficients out
	Vec<DEGREE() + 1> coefficients;
	for (size_t i = 0; i < DEGREE() + 1; ++i)
		coefficients[i] = augmentedMatrix[i][Columns(ATWA)];

	// write it out for use in the python script
	{
		FILE* file = nullptr;
		fopen_s(&file, "out/out.txt", "wb");
		fprintf(file,
			"    {\n"
			"        \"title\": \"\",\n"
			"        \"file\": \"\",\n"
			"        \"fitx\": ["
		);

		for (int i = 0; i < c_numPoints; ++i)
		{
			if (i > 0)
				fprintf(file, ", ");

			fprintf(file, "%f", dataPoints[i].x);
		}

		fprintf(file,
			"],\n"
			"        \"fity\": ["
		);

		for (int i = 0; i < c_numPoints; ++i)
		{
			if (i > 0)
				fprintf(file, ", ");

			fprintf(file, "%f", dataPoints[i].y);
		}

		fprintf(file,
			"],\n"
			"        \"fitweight\": ["
		);

		for (int i = 0; i < c_numPoints; ++i)
		{
			if (i > 0)
				fprintf(file, ", ");

			fprintf(file, "%f", dataPoints[i].weight);
		}
		
		fprintf(file,
			"],\n"
			"        \"constrainx\": ["
		);

		for (int i = 0; i < c_numConstraintPoints; ++i)
		{
			if (i > 0)
				fprintf(file, ", ");

			fprintf(file, "%f", constraintPoints[i].x);
		}

		fprintf(file,
			"],\n"
			"        \"constrainy\": ["
		);

		for (int i = 0; i < c_numConstraintPoints; ++i)
		{
			if (i > 0)
				fprintf(file, ", ");

			fprintf(file, "%f", constraintPoints[i].y);
		}

		fprintf(file,
			"],\n"
			"        \"fncoeffs\": ["
		);

		for (size_t i = 0; i < DEGREE() + 1; ++i)
		{
			if (i > 0)
				fprintf(file, ", ");

			fprintf(file, "%f", coefficients[i]);
		}

		fprintf(file,
			"],\n"
			"    }\n"
		);
		fclose(file);
	}
}

/*

TODO:
* compare weighted point vs constrainted point, both in a linear fit and a quadratic fit.
 * make images that show increasing weight, and compare vs an actual constriant

! can't define a quadratic by 2 points and a slope at midpoint. slope anywhere else is fine though.
* link to cls.pdf, the one from email, and also the one about piecewise linear regresion with lots of lines then merging them.
! could show generalized formula with symbols about summing powers times c_i
*/
