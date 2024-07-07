#pragma once


struct frenet_t;
struct CAGD_POINT;

typedef struct
{
  double     radius;
  int        is_center_defined;
  CAGD_POINT crv_pos;
  CAGD_POINT center;
  CAGD_POINT T_axis;
  CAGD_POINT N_axis;
} circle_data_t;

int get_num_circ_pnts( double radius );

void get_center_pnt( double param, circle_data_t *rp_circle_data );

void eval_circ( double         param,
                circle_data_t *p_circle_data,
                CAGD_POINT    *rp_out );

int draw_circle( double         param,
                 circle_data_t *p_circle_data,
                 int           *p_seg_id );
