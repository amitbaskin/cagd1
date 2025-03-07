#ifndef PARSE_FILE_H
#define PARSE_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "expr2tree.h"


void init_cur_crv();
void save_cur_crv( int x, int y, void *p_data );
void load_cur_crv( int x, int y, void *p_data );
void edit_cur_crv( int idx, char *p_str );

#endif // PARSE_FILE_H
