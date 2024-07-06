#pragma once


struct frenet_t;
struct CAGD_POINT;

#define NUM_OSC_PNTS 100

typedef struct
{
  double     radius;
  CAGD_POINT crv_pos;
  CAGD_POINT center;
  CAGD_POINT T_axis;
  CAGD_POINT N_axis;
} circle_data_t;


void get_center_pnt( double param, circle_data_t *rp_circle_data );

void eval_circ( double         param,
                circle_data_t *p_circle_data,
                CAGD_POINT    *rp_out );

int draw_circle( double         param,
                 circle_data_t *p_circle_data,
                 int           *p_seg_id );
