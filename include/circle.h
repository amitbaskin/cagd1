#pragma once


struct frenet_t;
struct CAGD_POINT;

#define NUM_OSC_PNTS 100

typedef struct
{
  double     radius;
  CAGD_POINT crv_pos;
  CAGD_POINT center;
  CAGD_POINT vec;
} circle_data_t;


void eval_circ( double                   param,
                double                   radius,
                const struct CAGD_POINT *p_center,
                const struct frenet_t   *p_frenet,
                CAGD_POINT              *rp_out );

void get_circle_data( double         param,
                      double        *p_radius,
                      frenet_t      *p_frenet,
                      circle_data_t *rp_crvtr_data );
