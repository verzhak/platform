#ifndef ALGO_H
#define ALGO_H
#include <all.h>

#define STATE_WAIT			0
#define STATE_WRITE			1
#define STATE_READ			2
#define STATE_WRITE_END		3

#define REG_STATE				0
#define REG_NUMBER_OF_CONTOUR	1
#define REG_NUMBER_OF_MATRICES	2

class Algo
{  
    int file;
    uint32_t * data;
    uint32_t * data_reg;
    const unsigned contour_size = 1024, matrix_size = 960, reg_size = 64;
    const unsigned size = reg_size + contour_size + matrix_size;
    const int offset_reg=16 ;
    const int offset_matr=256 ;
    const int count_element_matr=16;
    const int count_matr_in_block=15;

public:
    Algo();

    void read(const string fileName);
    vector<shared_ptr<uint32_t> > map_contour;
};

#endif // ALGO_H
