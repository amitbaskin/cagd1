#pragma once


struct frenet_t;

#define TRSN_DENSITY_SAMPLES 100

#define TRSN_CYCLES 10

int draw_helix( double param, struct frenet_t *frenet );
