#ifndef __BiaoShi_header_h__
#define __BiaoShi_header_h__

typedef float real;
const real pi = 3.1415926535897932384626433832795;
const int BUFFER_SIZE = 1000;

struct ShareEntity {
	real *mat;
	int *dui;
	int tot;
	ShareEntity *next;
	ShareEntity(real *matrix, int *con, int total = 0) {
		mat = matrix;
		dui = con;
		tot = total;
		next = NULL;
	}
};

FILE *logg;

#endif