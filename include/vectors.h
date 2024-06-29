#pragma once

#include "cagd.h"

void copy_vec( const CAGD_POINT *in, CAGD_POINT *out );
double vec_len( const CAGD_POINT *vec );
void scale_vec( double scale, CAGD_POINT *vec );
void normalize_vec( CAGD_POINT *vec );
double multiply_vecs( const CAGD_POINT *v1, const CAGD_POINT *v2 );
void cross_vecs( const CAGD_POINT *v1, const CAGD_POINT *v2, CAGD_POINT *out );
void diff_vecs( const CAGD_POINT *v1, const CAGD_POINT *v2, CAGD_POINT *out );
void add_vecs( const CAGD_POINT *v1, const CAGD_POINT *v2, CAGD_POINT *out );
