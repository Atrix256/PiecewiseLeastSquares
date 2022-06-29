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
#define DEGREE() 2
#define NUM_CURVES() 2

struct Constraint
{
	Vec<(DEGREE() + 1) * NUM_CURVES()> lhs;
	float rhs;
};

struct DataPoint
{
	float x = 0.0f;
	float y = 0.0f;
	float weight = 1.0f;
	int curveIndex = 0;
};

int main(int argc, char** argv)
{
	// The data points to fit
	DataPoint dataPoints[] =
	{
		{1.0f, 3.0f, 1.0f, 0},
		{2.0f, 4.0f, 1.0f, 0},
		{3.0f, 2.0f, 1.0f, 0},
		{3.0f, 2.0f, 1.0f, 1},
		{4.0f, 6.0f, 1.0f, 1},
		{5.0f, 7.0f, 1.0f, 1},
		{6.0f, 5.0f, 1.0f, 1},
	};
	static const int c_numPoints = _countof(dataPoints);

	// The constraint points
#if 0
	DataPoint constraintPoints[] =
	{
		{2.5f, 1.0f}
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
		//{{1.0f, 3.0f, -1.0f, -3.0f}, 0.0f},
		//{{0.0f, 1.0f, 0.0f, -1.0f}, 0.0f}
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
	Mtx<(DEGREE() + 1) * NUM_CURVES(), c_numPoints> A;
	for (auto& i : A)
		std::fill(i.begin(), i.end(), 0.0f);
	for (int y = 0; y < c_numPoints; ++y)
	{
		int xoffset = dataPoints[y].curveIndex * 2;
		for (int x = 0; x < DEGREE() + 1; ++x)
			A[y][x + xoffset] = powf(dataPoints[y].x, float(x));
	}
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
	Mtx<Columns(ATWA) + 1 + c_numConstraints, Rows(ATWA) + c_numConstraints> augmentedMatrix;
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
		for (int j = 0; j < (DEGREE() + 1) * NUM_CURVES(); ++j)
			augmentedMatrix[Columns(ATWA) + i][j] = constraints[i].lhs[j];

		// Put in the column
		for (int j = 0; j < (DEGREE() + 1) * NUM_CURVES(); ++j)
			augmentedMatrix[j][Rows(ATWA) + i] = constraints[i].lhs[j];

		// Put in the RHS
		augmentedMatrix[Columns(ATWA) + i][Rows(ATWA) + c_numConstraints] = constraints[i].rhs;
	}

	// Solve the equation for x:
	// A^TWA * x = A^TWY
	// x is the coefficients of the polynomial of our fit
	GaussJordanElimination(augmentedMatrix);

	// Get our coefficients out
	Vec<(DEGREE() + 1) * NUM_CURVES()> coefficients;
	for (size_t i = 0; i < coefficients.size(); ++i)
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

		for (int curveIndex = 0; curveIndex < NUM_CURVES(); ++curveIndex)
		{
			if (curveIndex == 0)
				fprintf(file, "[");
			else
				fprintf(file, ", [");

			for (size_t i = 0; i < DEGREE() + 1; ++i)
			{
				if (i > 0)
					fprintf(file, ", ");

				fprintf(file, "%f", coefficients[curveIndex * (DEGREE() + 1) + i]);
			}
			fprintf(file, "]");
		}

		fprintf(file,
			"],\n"
			"        \"ylim\":[],\n"
			"    }\n"
		);
		fclose(file);
	}

	// Show the equation. Note that the first coefficient is the constant, then degree 1, then degree 2, etc, so iterate in reverse order
	for (int curveIndex = 0; curveIndex < NUM_CURVES(); ++curveIndex)
	{
		int curveOffset = curveIndex * (DEGREE() + 1);

		bool first = true;
		printf("y = ");
		for (int degree = DEGREE(); degree >= 0; --degree)
		{
			if (!first)
				printf(" + ");

			if (degree > 0 && coefficients[curveOffset + degree] == 0.0f)
			{
				first = true;
				continue;
			}

			if (degree > 1)
				printf("%0.2fx^%i", coefficients[curveOffset + degree], degree);
			else if (degree == 1)
				printf("%0.2fx", coefficients[curveOffset + degree]);
			else
				printf("%0.2f", coefficients[curveOffset + degree]);

			first = false;
		}

		// Show how close the polynomial is to the data points given.
		printf("\n\n");
		for (int i = 0; i < c_numPoints; ++i)
		{
			float y = 0.0f;
			float x = 1.0f;
			for (int degree = 0; degree <= DEGREE(); ++degree)
			{
				y += coefficients[curveOffset+degree] * x;
				x *= dataPoints[i].x;
			}

			printf("data[%i]: (%f, %f) weight %0.2f, equation gives %f.  Error = %f\n", i, dataPoints[i].x, dataPoints[i].y, dataPoints[i].weight, y, y - dataPoints[i].y);
		}

		// Convert from power series polynomial to bernstein form aka a Bezier curves
		printf("\nBezier Control Points = [ ");
		Vec<DEGREE() + 1> controlPoints;
		{
			// Divide by binomial coefficients
			for (int i = 0; i < DEGREE() + 1; ++i)
				coefficients[curveOffset + i] /= BinomialCoefficient<float>(DEGREE(), float(i));

			// Do the reverse of making a difference table.
			for (int j = 0; j < DEGREE(); ++j)
			{
				controlPoints[j] = (float)coefficients[curveOffset + 0];

				for (int i = 0; i < DEGREE(); ++i)
					coefficients[i] += coefficients[curveOffset + i + 1];
				printf("%0.2f, ", controlPoints[j]);
			}
			controlPoints[DEGREE()] = (float)coefficients[curveOffset + 0];
			printf("%0.2f ]\n", controlPoints[DEGREE()]);
		}

		// Show the Bezier curve formula
		first = true;
		printf("\ny = f(t) = ");
		for (int degree = 0; degree <= DEGREE(); ++degree)
		{
			if (!first)
				printf(" + ");

			int bc = BinomialCoefficient<int>(DEGREE(), degree);
			if (bc != 1)
				printf("%i*", bc);
			printf("%0.2f", controlPoints[degree]);

			int degree1MinusT = DEGREE() - degree;
			int degreeT = degree;

			if (degree1MinusT > 1)
				printf("(1-t)^%i", degree1MinusT);
			else if (degree1MinusT == 1)
				printf("(1-t)");

			if (degreeT > 1)
				printf("t^%i", degreeT);
			else if (degreeT == 1)
				printf("t");

			first = false;
		}
		printf("\n");
	}

}

/*

TODO:

* print out polynomial? so it's useful to humans too.

! can't define a quadratic by 2 points and a slope at midpoint. slope anywhere else is fine though.
* link to cls.pdf, the one from email, and also the one about piecewise linear regresion with lots of lines then merging them.
! could show generalized formula with symbols about summing powers times c_i
*/
