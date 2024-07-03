#include <stdio.h>
#include <math.h>
#include "vectors.h"
#include "cur_crv.h"


int scale_not_zero( double scale )
{
  return fabs( scale ) > EPSILON;
}


int vec_3d_not_zero( const CAGD_POINT *vec )
{
  int status = 1;

  status = status && scale_not_zero( vec->x );
  status = status && scale_not_zero( vec->y );
  status = status && scale_not_zero( vec->z );

  return status;
}


void copy_vec( const CAGD_POINT *in, CAGD_POINT *out )
{
  out->x = in->x;
  out->y = in->y;
  out->z = in->z;
}


double vec_len( const CAGD_POINT *vec )
{
  double sum = vec->x * vec->x +
               vec->y * vec->y +
               vec->z * vec->z;;

  return sqrt( sum );
}


void scale_vec( double scale, CAGD_POINT *vec )
{
  vec->x *= scale;
  vec->y *= scale;
  vec->z *= scale;
}


void normalize_vec( CAGD_POINT *vec )
{
  double length = vec_len( vec );

  vec->x /= length;
  vec->y /= length;
  vec->z /= length;
}


void diff_vecs( const CAGD_POINT *v1, const CAGD_POINT *v2, CAGD_POINT *out )
{
  out->x = v1->x - v2->x;
  out->y = v1->y - v2->y;
  out->z = v1->z - v2->z;
}


void add_vecs( const CAGD_POINT *v1, const CAGD_POINT *v2, CAGD_POINT *out )
{
  out->x = v1->x + v2->x;
  out->y = v1->y + v2->y;
  out->z = v1->z + v2->z;
}


double multiply_vecs( const CAGD_POINT *v1, const CAGD_POINT *v2 )
{
  return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}


void cross_vecs( const CAGD_POINT *v1, const CAGD_POINT *v2, CAGD_POINT *out )
{
  /*
  |ii|jj|kk|
  |ax|ay|az|
  |bx|by|bz|
  */
  out->x = v1->y * v2->z - v1->z * v2->y;
  out->y = v1->z * v2->x - v1->x * v2->z;
  out->z = v1->x * v2->y - v1->y * v2->x;
}
