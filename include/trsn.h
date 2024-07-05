#pragma once


struct frenet_t;

#define HELIX_DENSITY_SAMPLES 100

#define HELIX_CYCLES 3


int draw_helix( double param, struct frenet_t *p_frenet );
