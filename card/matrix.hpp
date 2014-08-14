
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "all.hpp"

class CMatrix
{
	double __mtx[4][4];

	public:

		CMatrix();
		CMatrix(const CMatrix & mtx);

		static CMatrix eye();

		void rotate(const double angle, const double x, const double z, const double y);
		void translate(const double x, const double z, const double y);
		void operator*=(const CMatrix & op);

		inline double & operator[](const unsigned ind) { return __mtx[ind / 4][ind % 4]; };
		inline void pitch(const double angle) { rotate(- angle, 1, 0, 0); };
		inline void roll(const double angle) { rotate(angle, 0, 0, 1); };
		inline void course(const double angle) { rotate(angle + 90, 0, 1, 0); }; // Докрутка на север
};

#endif

