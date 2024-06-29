#include "cagd.h"
#include "cur_crv.h"


typedef struct
{
  CAGD_POINT pos;
  CAGD_POINT csys[ 3 ]; // T, N, B
  CAGD_POINT frenet[ 3 ];
  double crvtr;
  double trsn;
} frenet_t;

frenet_t frenet;


void calc_frenet( double param, frenet_t *frenet );
void draw_frenet( double param, frenet_t *frenet );
