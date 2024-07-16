#include <errno.h>
#include "load_cur_crv.h"
#include "cur_crv.h"
#include "color.h"
#include "vectors.h"


extern int num_samples;


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
* init_cur_crv
******************************************************************************/
void init_cur_crv()
{
  int is_error = FALSE;

  e2t_expr_node *tree = NULL;

  free_all_segs( TRUE, TRUE );
  free_all_trees();

  if( double_cmp( cur_crv.domain[ 0 ], cur_crv.domain[ 1 ] ) > 0 )
  {
    is_error = TRUE;
    print_err( "Error: Invalid tmin and / or tmax values.\n" );
  }

  for( int i = 0; is_error == FALSE && i < SPACE_DIM; ++i )
  {
    if( IS_DEBUG )
      printf( "\n\nVariable #%d is: %s\n\n", i, cur_crv.expressions[ i ] );

    tree = e2t_expr2tree( cur_crv.expressions[ i ] );

    if( tree == NULL )
    {
      char msg[ MAX_LINE_LENGTH ];
      sprintf( msg, "Error: failed to parse variable #%d.\n", i );
      print_err( msg );
      is_error = 1;
    }
    else
      cur_crv.trees[ i ][ 0 ] = tree;

    for( int j = 1; is_error == FALSE && j < DERIVATIVE_LEVEL; ++j )
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
        cur_crv.trees[ i ][ j ] = tree;
    }
  }

  if( is_error == TRUE )
    cur_crv.defined = FALSE;
  else
  {
    cur_crv.defined = TRUE;
    cur_crv.offset = DEFAULT_OFFSET;
    draw_cur_crv( num_samples );
  }
}


/******************************************************************************
* edit_cur_crv
******************************************************************************/
void edit_cur_crv( int idx, char *p_str )
{
  for( int i = 0; i < MAX_LINE_LENGTH; ++i )
    cur_crv.expressions[ idx ][ i ] = '\0';

  strcpy( cur_crv.expressions[ idx ], p_str );
}


/******************************************************************************
* save_cur_crv
******************************************************************************/
void save_cur_crv( int dummy1, int dummy2, void *p_data )
{
  const char *file_path = ( const char * )p_data;

  FILE *file = fopen( file_path, "w" );

  if( file == NULL )
  {
    fprintf( stderr, "Error: Could not open file %s for writing\n",
             file_path );
    return;
  }

  for( int i = 0; i < SPACE_DIM; ++i )
    fprintf( file, "%s\n", cur_crv.expressions[ i ] );

  fprintf( file, "%lf %lf\n", cur_crv.domain[ 0 ], cur_crv.domain[ 1 ] );

  fclose( file );

  if( IS_DEBUG )
    printf( "Curve data successfully saved to %s\n", file_path );
}


/******************************************************************************
* load_cur_crv
******************************************************************************/
void load_cur_crv( int dummy1, int dummy2, void *p_data )
{
  FILE *file = NULL;

  int is_error   = FALSE;
  int line_count = 0;
  int line_len   = 0;

  char line[ MAX_LINE_LENGTH ];

  clean_cur_crv();

  file = fopen( ( char * )p_data, "r" );

  if( file == NULL )
  {
    print_err( "Error opening file" );
    is_error = 1;
  }

  while( is_error == FALSE          &&
         line_count < SPACE_DIM + 1 &&
         fgets( line, sizeof( line ), file ) )
  {
    if( IS_DEBUG )
      printf( "\nProcessing line: %s\n", line );

    trim_whitespaces( line );

    if( line[ 0 ] != '\0' && line[ 0 ] != '#' )
    {
      ++line_count;

      if( IS_DEBUG )
        printf( "Good line: %s\n", line );

      if( line_count <= SPACE_DIM )
      {
        line_len = strlen( line );
        strncpy( cur_crv.expressions[ line_count - 1 ], line, line_len );
        cur_crv.expressions[ line_count - 1 ][ line_len ] = '\0';
      }
      else if( line_count == SPACE_DIM + 1 )
      {
        if( sscanf( line,
                    "%lf %lf",
                    &cur_crv.domain[ 0 ],
                    &cur_crv.domain[ 1 ] ) != 2 ||
            double_cmp( cur_crv.domain[ 0 ], cur_crv.domain[ 1 ] ) > 0 )
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

  if( is_error == FALSE && line_count < SPACE_DIM + 1 )
  {
    print_err( "Error: Missing required lines in the input file.\n" );
    is_error = TRUE;
  }

  if( is_error == FALSE )
    init_cur_crv();
  else
    clean_cur_crv();
}
