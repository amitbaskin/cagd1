#pragma once

#include "cagd.h"

#define EPSILON 1e-9


int scale_not_zero( double scale );

int vec_not_zero( const CAGD_POINT *p_vec );

void copy_vec( const CAGD_POINT *p_in, CAGD_POINT *rp_out );

double vec_len( const CAGD_POINT *p_vec );

void scale_vec( double scale, CAGD_POINT *p_vec );

void normalize_vec( CAGD_POINT *p_vec );

double multiply_vecs( const CAGD_POINT *p_v1, const CAGD_POINT *p_v2 );

void cross_vecs( const CAGD_POINT *p_v1,
                 const CAGD_POINT *p_v2,
                 CAGD_POINT       *rp_out );

void diff_vecs( const CAGD_POINT *p_v1,
                const CAGD_POINT *p_v2,
                CAGD_POINT       *rp_out );

void add_vecs( const CAGD_POINT *p_v1,
               const CAGD_POINT *p_v2,
               CAGD_POINT       *rp_out );
