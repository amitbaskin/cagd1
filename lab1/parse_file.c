#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "expr2tree.h"

// Function to trim leading and trailing whitespaces
char *trim_whitespace( char *str )
{
  char *end;

  // Trim leading space
  while( isspace( ( unsigned char ) *str ) ) str++;

  if( *str == 0 ) // All spaces
    return str;

  // Trim trailing space
  end = str + strlen( str ) - 1;
  while( end > str && isspace( ( unsigned char ) *end ) ) end--;

  // Write new null terminator
  *(end + 1) = 0;

  return str;
}

int parse_file( int dummy1, int dummy2, void *p_data )
{
  int status = 0;

  FILE *file = fopen( ( char * )p_data, "r" );

  if( file == NULL )
  {
    perror( "Error opening file" );
    return 1;
  }

  char line[ 256 ];
  int line_count = 0;
  char x_form[ 256 ], y_form[ 256 ], z_form[ 256 ];
  double tmin, tmax;

  while( fgets( line, sizeof( line ), file ) )
  {
    char *trimmed_line = trim_whitespace( line );

    // Skip comments and empty lines
    if( trimmed_line[ 0 ] == '#' || strlen( trimmed_line ) == 0 )
    {
      continue;
    }

    ++line_count;

    switch( line_count )
    {
      case 1:
        strncpy( x_form, trimmed_line, sizeof( x_form ) );
        x_form[ sizeof( x_form ) - 1 ] = '\0';  // Ensure null-termination
        break;
      case 2:
        strncpy( y_form, trimmed_line, sizeof( y_form ) );
        y_form[ sizeof( y_form ) - 1 ] = '\0';  // Ensure null-termination
        break;
      case 3:
        strncpy( z_form, trimmed_line, sizeof( z_form ) );
        z_form[ sizeof( z_form ) - 1 ] = '\0';  // Ensure null-termination
        break;
      case 4:
        if( sscanf( trimmed_line, "%lf %lf", &tmin, &tmax ) != 2 || tmin > tmax )
        {
          fprintf( stderr, "Error: Invalid tmin and tmax values.\n" );
          fclose( file );
          return 1;
        }
        break;
      default:
        break;
    }
  }

  fclose( file );

  // Check if we have all required items
  if( line_count < 4 || strlen( x_form ) == 0 || strlen( y_form ) == 0 || strlen( z_form ) == 0 )
  {
    fprintf( stderr, "Error: Missing required lines in the input file.\n" );
    return 1;
  }

  // Output the parsed data
  printf( "X(t): %s\n", x_form );
  printf( "Y(t): %s\n", y_form );
  printf( "Z(t): %s\n", z_form );
  printf( "tmin: %f, tmax: %f\n", tmin, tmax );

  e2t_expr_node *tree;
  e2t_expr_node *dtree;
  double x;
  char s[ 128 ];

  tree = e2t_expr2tree( x_form );

  if( !tree )
  {
    printf( "Error %d\n", e2t_parsing_error );
  }

  printf( "The tree is:" );
  e2t_printtree( tree, ( char * ) NULL );

  printf( "\n\nDtree/Dx is:" );
  dtree = e2t_derivtree( tree, E2T_PARAM_T );
  e2t_printtree( dtree, ( char * ) NULL );
  printf( "   == %lf\n\n", e2t_evaltree( tree ) );

  printf( "Enter x value:" );
  gets( s );
  sscanf( s, "%lf", &x );
  e2t_setparamvalue( x, E2T_PARAM_T );
  printf( "Tree value for x = %lf is %lf\n", x, e2t_evaltree( tree ) );

  return status;
}
