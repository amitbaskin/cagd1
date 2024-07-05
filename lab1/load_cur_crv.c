#include <errno.h>
#include "load_cur_crv.h"
#include "cur_crv.h"


extern int num_samples;

#define MAX_LINE_LENGTH 1024


/******************************************************************************
* trim_whitespaces
******************************************************************************/
void trim_whitespaces( char *str )
{
  int length = strlen( str );
  int start = 0;
  int end = length - 1;

  // Trim leading whitespace
  while( start <= end && isspace( ( unsigned char ) str[ start ] ) )
    start++;

  // Trim trailing whitespace
  while( end >= start && isspace( ( unsigned char ) str[ end ] ) )
    end--;

  // Shift trimmed string to the beginning
  memmove( str, str + start, end - start + 1 );
  str[ end - start + 1 ] = '\0';
}


/******************************************************************************
* validate_tree
******************************************************************************/
void validate_tree( int i, int j )
{
  e2t_expr_node *tree = cur_crv.trees[ i ][ j ];

  if( tree != NULL )
  {
    printf( "\nThe %d, %d tree is:\n", i, j );

    e2t_printtree( tree, ( char * ) NULL );

    e2t_setparamvalue( cur_crv.domain[ 0 ], E2T_PARAM_T );

    printf( "\n\nTree value for t = %lf is %lf\n",
            cur_crv.domain[ 0 ],
            e2t_evaltree( tree ) );

    e2t_setparamvalue( cur_crv.domain[ 1 ], E2T_PARAM_T );

    printf( "\nTree value for t = %lf is %lf\n",
            cur_crv.domain[ 1 ],
            e2t_evaltree( tree ) );
  }
}


/******************************************************************************
* init_cur_crv
******************************************************************************/
static void
init_cur_crv( char   variables_string[ SPACE_DIM ][ MAX_LINE_LENGTH ],
              double tmin,
              double tmax )
{
  int is_error = 0;

  e2t_expr_node *tree = NULL;

  cur_crv.defined = 1;

  cur_crv.domain[ 0 ] = tmin;
  cur_crv.domain[ 1 ] = tmax;

  for( int i = 0; !is_error && i < SPACE_DIM; ++i )
  {
    printf( "\n\nVariable #%d is: %s\n\n", i, variables_string[ i ] );

    tree = e2t_expr2tree( variables_string[ i ] );

    if( tree == NULL )
    {
      char msg[ MAX_LINE_LENGTH ];
      sprintf( msg, "Error: failed to parse variable #%d.\n", i );
      print_err( msg );
      is_error = 1;
    }
    else
    {
      cur_crv.trees[ i ][ 0 ] = tree;
      validate_tree( i, 0 );
    }

    for( int j = 1; !is_error && j < DERIVATIVE_LEVEL; ++j )
    {
      tree = e2t_derivtree( cur_crv.trees[ i ][ j - 1 ], E2T_PARAM_T );

      if( tree == NULL )
      {
        char msg[ MAX_LINE_LENGTH ];
        sprintf( msg, "Error: failed to calculate derivative #%d of variable "
                 "# % d .\n", j, i );
        print_err( msg );
        is_error = 1;
      }
      else
      {
        cur_crv.trees[ i ][ j ] = tree;
        validate_tree( i, j );
      }
    }
  }

  if( is_error )
    clean_cur_crv();
  else
    draw_cur_crv( num_samples );
}


/******************************************************************************
* load_cur_crv
******************************************************************************/
void load_cur_crv( int dummy1, int dummy2, void *p_data )
{
  FILE *file = NULL;

  int is_error   = 0;
  int line_count = 0;
  int line_len   = 0;

  double tmin = 0, tmax = 0;

  char line[ MAX_LINE_LENGTH ];
  char variables_string[ SPACE_DIM ][ MAX_LINE_LENGTH ];

  clean_cur_crv();

  file = fopen( ( char * )p_data, "r" );

  if( file == NULL )
  {
    print_err( "Error opening file" );
    is_error = 1;
  }

  for( int i = 0; i < SPACE_DIM; ++i )
    variables_string[ i ][ 0 ] = '\0';

  while( !is_error                   &&
          line_count < SPACE_DIM + 1 &&
          fgets( line, sizeof( line ), file ) )
  {
    printf( "\nProcessing line: %s\n", line );

    trim_whitespaces( line );

    if( line[ 0 ] != '\0' && line[ 0 ] != '#' )
    {
      ++line_count;

      printf( "Good line: %s\n", line );

      if( line_count <= SPACE_DIM )
      {
        line_len = strlen( line );
        strncpy( variables_string[ line_count - 1 ], line, line_len );
        variables_string[ line_count - 1 ][ line_len ] = '\0';
      }
      else if( line_count == SPACE_DIM + 1 )
      {
        if( sscanf( line, "%lf %lf", &tmin, &tmax ) != 2 || tmin > tmax )
        {
          print_err( "Error: Invalid tmin and / or tmax values.\n" );
          fclose( file );
          is_error = 1;
        }
      }
    }
  }

  if( file != NULL )
    fclose( file );

  if( !is_error && line_count < SPACE_DIM + 1 )
  {
    fprintf( stderr, "Error: Missing required lines in the input file.\n" );
    is_error = 1;
  }

  if( !is_error )
    init_cur_crv( variables_string, tmin, tmax );
}
