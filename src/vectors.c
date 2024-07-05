#include <stdio.h>
#include <math.h>
#include "vectors.h"
#include "cur_crv.h"


int scale_not_zero( double scale )
{
  return fabs( scale ) > EPSILON;
}


int vec_3d_not_zero( const CAGD_POINT *p_vec )
{
  int status = 1;

  status = status && scale_not_zero( p_vec->x );
  status = status && scale_not_zero( p_vec->y );
  status = status && scale_not_zero( p_vec->z );

  return status;
}


void copy_vec( const CAGD_POINT *p_in, CAGD_POINT *rp_out )
{
  rp_out->x = p_in->x;
  rp_out->y = p_in->y;
  rp_out->z = p_in->z;
}


double vec_len( const CAGD_POINT *p_vec )
{
  double sum = p_vec->x * p_vec->x +
               p_vec->y * p_vec->y +
               p_vec->z * p_vec->z;;

  return sqrt( sum );
}


void scale_vec( double scale, CAGD_POINT *p_vec )
{
  p_vec->x *= scale;
  p_vec->y *= scale;
  p_vec->z *= scale;
}


void normalize_vec( CAGD_POINT *p_vec )
{
  double length = vec_len( p_vec );

  p_vec->x /= length;
  p_vec->y /= length;
  p_vec->z /= length;
}


void diff_vecs( const CAGD_POINT *p_v1, const CAGD_POINT *p_v2, CAGD_POINT *rp_out )
{
  rp_out->x = p_v1->x - p_v2->x;
  rp_out->y = p_v1->y - p_v2->y;
  rp_out->z = p_v1->z - p_v2->z;
}


void add_vecs( const CAGD_POINT *p_v1, const CAGD_POINT *p_v2, CAGD_POINT *rp_out )
{
  rp_out->x = p_v1->x + p_v2->x;
  rp_out->y = p_v1->y + p_v2->y;
  rp_out->z = p_v1->z + p_v2->z;
}


double multiply_vecs( const CAGD_POINT *p_v1, const CAGD_POINT *p_v2 )
{
  return p_v1->x * p_v2->x + p_v1->y * p_v2->y + p_v1->z * p_v2->z;
}


void cross_vecs( const CAGD_POINT *p_v1, const CAGD_POINT *p_v2, CAGD_POINT *rp_out )
{
  /*
  |ii|jj|kk|
  |ax|ay|az|
  |bx|by|bz|
  */
  rp_out->x = p_v1->y * p_v2->z - p_v1->z * p_v2->y;
  rp_out->y = p_v1->z * p_v2->x - p_v1->x * p_v2->z;
  rp_out->z = p_v1->x * p_v2->y - p_v1->y * p_v2->x;
}
