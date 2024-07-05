#pragma once


struct frenet_t;
struct CAGD_POINT;

typedef struct
{
  double     radius;
  CAGD_POINT crv_pos;
  CAGD_POINT center;
  CAGD_POINT vec;
} crvtr_data_t;

#define NUM_OSC_PNTS 100

void get_crvtr_data( double        param,
                     frenet_t     *p_frenet,
                     crvtr_data_t *rp_crvtr_data );

int draw_osc_circle( double param, struct frenet_t *p_frenet );
