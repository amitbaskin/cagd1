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
* get_scale_inv_or_zero
******************************************************************************/
double get_scale_inv_or_zero( double scale )
{
  return scale_not_zero( scale ) ? 1 / scale : 0.0;
}


/******************************************************************************
* scale_div_vec
******************************************************************************/
void scale_div_vec( double denom, CAGD_POINT *rp_out )
{
  denom = get_scale_inv_or_zero( denom );
  scale_vec( denom, rp_out );
}


/******************************************************************************
* normalize_vec
******************************************************************************/
void normalize_vec( CAGD_POINT *p_vec )
{
  scale_div_vec( vec_len( p_vec ), p_vec );
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
  int status = FALSE;

  status = status || scale_not_zero( p_vec->x );
  status = status || scale_not_zero( p_vec->y );
  status = status || scale_not_zero( p_vec->z );

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


/******************************************************************************
* rotate_vec
******************************************************************************/
void rotate_vec( double      angle,
                 CAGD_POINT *p_in,
                 CAGD_POINT *p_rot,
                 CAGD_POINT *p_out )
{
  CAGD_POINT trans;

  //normalize_vec( p_rot );

  double sin_a = sin( angle );
  double cos_a = cos( angle );
  double n_cos_a = 1 - cos( angle );

  double ux = p_rot->x;
  double uy = p_rot->x;
  double uz = p_rot->x;

  double uxux = pow( p_rot->x, 2 );
  double uyuy = pow( p_rot->y, 2 );
  double uzuz = pow( p_rot->z, 2 );

  double uxuy = ux * uy;
  double uxuz = ux * uz;
  double uyuz = uy * uz;

  copy_vec( p_in, &trans );

  // translation
  //diff_vecs( p_in, p_rot, &trans );

  p_out->x = ( cos_a + uxux * n_cos_a )      * trans.x +
             ( uxuy * n_cos_a - uz * sin_a ) * trans.y +
             ( uxuz * n_cos_a + uy * sin_a ) * trans.z;

  p_out->y = ( uxuy * n_cos_a + uz * sin_a ) * trans.x +
             ( cos_a + uyuy * n_cos_a )      * trans.y +
             ( uyuz * n_cos_a - ux * sin_a ) * trans.z;

  p_out->z = ( uxuz * n_cos_a - uy * sin_a ) * trans.x +
             ( uyuz * n_cos_a + ux * sin_a ) * trans.y +
             ( cos_a + uzuz * n_cos_a )      * trans.z;

  // retranslation
  //add_vecs( p_out, p_rot, p_out );

  //normalize_vec( p_out );
}
