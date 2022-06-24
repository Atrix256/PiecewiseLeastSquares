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

struct Constraint
{
	Vec<DEGREE() + 1> lhs;
	float rhs;
};

struct DataPoint
{
	float x = 0.0f;
	float y = 0.0f;
	float weight = 1.0f;
};

int main(int argc, char** argv)
{
	// The data points to fit
	DataPoint dataPoints[] =
	{
		{1.0f, 2.0f, 1.0f},
		//{2.0f, 4.0f, 100.0f},
		{3.0f, 3.0f, 1.0f},
	};
	static const int c_numPoints = _countof(dataPoints);

	// The constraint points
#if 1
	DataPoint constraintPoints[] =
	{
		{2.0f, 4.0f}
	};
	static const int c_numConstraintPoints = _countof(constraintPoints);
#else
	DataPoint* constraintPoints = nullptr;
	static const int c_numConstraintPoints = 0;
#endif

	// The constraint values
	// LHS is what the polynomial coefficients are multiplied by (like, powers of x, just like dataPoints)
	// RHS is what the result of that multiplication should be (like, the y value of a datapoint)
#if 0
	Constraint constraintValues[] =
	{
		{{0.0f, 0.0f}, 0.0f}
	};
	static const int c_numConstraintValues = _countof(constraintValues);
#else
	Constraint* constraintValues = nullptr;
	static const int c_numConstraintValues = 0;
#endif

	// unify the constraint data
	static const int c_numConstraints = c_numConstraintPoints + c_numConstraintValues;
	std::array<Constraint, c_numConstraints> constraints;
	{
		for (int i = 0; i < c_numConstraintPoints; ++i)
		{
			float x = 1.0f;
			for (int j = 0; j < DEGREE() + 1; ++j)
			{
				constraints[i].lhs[j] = x;
				x *= constraintPoints[i].x;
			}
			constraints[i].rhs = constraintPoints[i].y;
		}

		for (int i = 0; i < c_numConstraintValues; ++i)
			constraints[c_numConstraintPoints + i] = constraintValues[i];
	}

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
	Mtx<Columns(ATWA) + 1 + c_numConstraints, DEGREE() + 1 + c_numConstraints> augmentedMatrix;
	for (auto& i : augmentedMatrix)
		std::fill(i.begin(), i.end(), 0.0f);

	for (size_t iy = 0; iy < Rows(ATWA); ++iy)
	{
		for (size_t ix = 0; ix < Columns(ATWA); ++ix)
			augmentedMatrix[iy][ix] = ATWA[iy][ix];

		augmentedMatrix[iy][Columns(ATWA) + c_numConstraints] = ATWY[iy];
	}

	// Put the constraints in
	for (int i = 0; i < c_numConstraints; ++i)
	{
		// Put in the row
		for (int j = 0; j < DEGREE() + 1; ++j)
			augmentedMatrix[Columns(ATWA) + i][j] = constraints[i].lhs[j];

		// Put in the column
		for (int j = 0; j < DEGREE() + 1; ++j)
			augmentedMatrix[j][DEGREE() + 1 + i] = constraints[i].lhs[j];

		// Put in the RHS
		augmentedMatrix[Columns(ATWA) + i][DEGREE() + 1 + c_numConstraints] = constraints[i].rhs;
	}

	// Solve the equation for x:
	// A^TWA * x = A^TWY
	// x is the coefficients of the polynomial of our fit
	GaussJordanElimination(augmentedMatrix);

	// Get our coefficients out
	Vec<DEGREE() + 1> coefficients;
	for (size_t i = 0; i < DEGREE() + 1; ++i)
		coefficients[i] = augmentedMatrix[i][Columns(ATWA) + c_numConstraints];

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
			"        \"ylim\":[],\n"
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
