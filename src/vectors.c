#include <stdio.h>
#include <math.h>
#include "vectors.h"
#include "cur_crv.h"


/******************************************************************************
* scale_not_zero
******************************************************************************/
int scale_not_zero( double scale )
{
  return fabs( scale ) > EPSILON;
}


/******************************************************************************
* double_cmp
******************************************************************************/
int double_cmp( double scale_1, double scale_2 )
{
  double diff = scale_1 - scale_2;

  int is_zero = !scale_not_zero( diff );

  return is_zero ? 0 : diff > EPSILON ? 1 : -1;
}


/******************************************************************************
* is_scale_initialized
******************************************************************************/
int is_scale_initialized( double scale )
{
  return double_cmp( scale, -HUGE_DOUBLE );
}


/******************************************************************************
* vec_not_zero
******************************************************************************/
int vec_not_zero( const CAGD_POINT *p_vec )
{
  int status = 1;

  status = status && scale_not_zero( p_vec->x );
  status = status && scale_not_zero( p_vec->y );
  status = status && scale_not_zero( p_vec->z );

  return status;
}


/******************************************************************************
* copy_vec
******************************************************************************/
void copy_vec( const CAGD_POINT *p_in, CAGD_POINT *rp_out )
{
  rp_out->x = p_in->x;
  rp_out->y = p_in->y;
  rp_out->z = p_in->z;
}


/******************************************************************************
* vec_len
******************************************************************************/
double vec_len( const CAGD_POINT *p_vec )
{
  double sum = p_vec->x * p_vec->x +
               p_vec->y * p_vec->y +
               p_vec->z * p_vec->z;;

  return sqrt( sum );
}


/******************************************************************************
* scale_vec
******************************************************************************/
void scale_vec( double scale, CAGD_POINT *p_vec )
{
  p_vec->x *= scale;
  p_vec->y *= scale;
  p_vec->z *= scale;
}


/******************************************************************************
* normalize_vec
******************************************************************************/
void normalize_vec( CAGD_POINT *p_vec )
{
  double length = vec_len( p_vec );

  p_vec->x /= length;
  p_vec->y /= length;
  p_vec->z /= length;
}


/******************************************************************************
* diff_vecs
******************************************************************************/
void diff_vecs( const CAGD_POINT *p_v1,
                const CAGD_POINT *p_v2,
                CAGD_POINT       *rp_out )
{
  rp_out->x = p_v1->x - p_v2->x;
  rp_out->y = p_v1->y - p_v2->y;
  rp_out->z = p_v1->z - p_v2->z;
}


/******************************************************************************
* add_vecs
******************************************************************************/
void add_vecs( const CAGD_POINT *p_v1,
               const CAGD_POINT *p_v2,
               CAGD_POINT       *rp_out )
{
  rp_out->x = p_v1->x + p_v2->x;
  rp_out->y = p_v1->y + p_v2->y;
  rp_out->z = p_v1->z + p_v2->z;
}


/******************************************************************************
* multiply_vecs
******************************************************************************/

double multiply_vecs( const CAGD_POINT *p_v1, const CAGD_POINT *p_v2 )
{
  return p_v1->x * p_v2->x + p_v1->y * p_v2->y + p_v1->z * p_v2->z;
}


/******************************************************************************
* cross_vecs
******************************************************************************/
void cross_vecs( const CAGD_POINT *p_v1,
                 const CAGD_POINT *p_v2,
                 CAGD_POINT       *rp_out )
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
