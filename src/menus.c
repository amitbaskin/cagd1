#include <cagd.h>
#include <stdio.h>
#include "menus.h"
#include "resource.h"
#include "frenet.h"
#include "evolute_n_offset.h"
#include "color.h"
#include "crvtr.h"
#include "trsn.h"
#include "sphere.h"
#include "load_cur_crv.h"

char myBuffer[BUFSIZ];
UINT myText;
HMENU g_lmb_menu = NULL;
HMENU g_options_menu = NULL;
HMENU g_animation_menu = NULL;
HMENU g_offset_menu = NULL;
HMENU g_cur_curve_menu = NULL;

extern HMENU g_anim_settings_menu;
extern int num_samples;
extern int frenet_anim_running;
extern double frenet_anim_speed;
extern int frenet_anim_smoothness;

extern void myMessage( PSTR title, PSTR message, UINT type );

/******************************************************************************
* init_menus
******************************************************************************/
void init_menus()
{
  HMENU op_menu = CreatePopupMenu(); // options
  HMENU fre_menu = CreatePopupMenu(); // frenet
  HMENU lmb_menu = CreatePopupMenu(); // lmb sub menu
  HMENU anim_settings_menu = CreatePopupMenu(); // animation settings menu
  HMENU offset_menu = CreatePopupMenu(); // animation settings menu
  HMENU cur_curve_menu = CreatePopupMenu(); // animation settings menu

  g_lmb_menu = lmb_menu;
  g_anim_settings_menu = anim_settings_menu;
  g_options_menu = op_menu;
  g_animation_menu = fre_menu;
  g_offset_menu = offset_menu;
  g_cur_curve_menu = cur_curve_menu;

  // options
  AppendMenu( op_menu, MF_STRING, CAGD_SEGS, "Change Refinement" );
  AppendMenu( op_menu, MF_STRING, CAGD_SHOW_EVOLUTE_MENU, "Show Evolute Curve" );
  AppendMenu( op_menu, MF_STRING | MF_POPUP, ( UINT_PTR )offset_menu, "Offset Curve" );
  AppendMenu( op_menu, MF_STRING | MF_POPUP, ( UINT_PTR )lmb_menu, "Left Mouse button" );
  AppendMenu( op_menu, MF_STRING | MF_POPUP, ( UINT_PTR )anim_settings_menu, "Animation Settings" );
  AppendMenu( op_menu, MF_STRING | MF_POPUP, ( UINT_PTR )cur_curve_menu, "Current Curve Settings" );
  AppendMenu( op_menu, MF_SEPARATOR, 0, NULL );
  AppendMenu( op_menu, MF_STRING, CAGD_REDRAW_ALL, "Redraw All" );

  // frenet
  AppendMenu( fre_menu, MF_STRING, CAGD_FRENET_ANIM_START, "Start Animation" );
  AppendMenu( fre_menu, MF_STRING | MF_GRAYED, CAGD_FRENET_ANIM_STOP, "Stop Animation" );

  // lmb sub menu
  AppendMenu( lmb_menu, MF_STRING | MF_CHECKED, CAGD_LMB_FRENET_MENU, "Show Frenet Frame" );
  AppendMenu( lmb_menu, MF_STRING, CAGD_LMB_OSCULATING_MENU, "Show Osculating Circle" );
  AppendMenu( lmb_menu, MF_STRING, CAGD_LMB_TORSION_MENU, "Show Torsion Helix" );
  AppendMenu( lmb_menu, MF_STRING, CAGD_LMB_SPHERE_MENU, "Show Osculating Sphere" );

  // animation settings menu
  AppendMenu( anim_settings_menu, MF_STRING | MF_CHECKED, CAGD_ANIM_FRENET_MENU, "Animate Frenet Frame" );
  AppendMenu( anim_settings_menu, MF_STRING, CAGD_ANIM_OSCULATING_MENU, "Animate Osculating Circle" );
  AppendMenu( anim_settings_menu, MF_STRING, CAGD_ANIM_TORSION_MENU , "Animate Torsion Helix" );
  AppendMenu( anim_settings_menu, MF_STRING, CAGD_ANIM_SPHERE_MENU, "Animate Osculating Sphere" );
  AppendMenu( anim_settings_menu, MF_SEPARATOR, 0, NULL );
  AppendMenu( anim_settings_menu, MF_STRING, CAGD_FRENET_ANIM_SPEED, "Set Animation Speed" );

  // Offset curve sub menu
  AppendMenu( offset_menu, MF_STRING, CAGD_SHOW_OFFSET_CURVE_MENU, "Show Offset Curve" );
  AppendMenu( offset_menu, MF_SEPARATOR, 0, NULL );
  AppendMenu( offset_menu, MF_STRING, CAGD_OFFSET_CURVE_VALUE_MENU, "Set Offset Value" );

  // Current Curve settings sub menu
  AppendMenu( cur_curve_menu, MF_STRING, CAGD_X_PARAMETER_MENU, "Change X(T) expression" );
  AppendMenu( cur_curve_menu, MF_STRING, CAGD_Y_PARAMETER_MENU, "Change Y(T) expression" );
  AppendMenu( cur_curve_menu, MF_STRING, CAGD_Z_PARAMETER_MENU, "Change Z(T) expression" );
  AppendMenu( cur_curve_menu, MF_SEPARATOR, 0, NULL );
  AppendMenu( cur_curve_menu, MF_STRING, CAGD_LOWER_DOMAIN_MENU, "Change minimum t value" );
  AppendMenu( cur_curve_menu, MF_STRING, CAGD_HIGHER_DOMAIN_MENU, "Change maximum t value" );


  // adding to cagd
  cagdAppendMenu( fre_menu, "Animation" );
  cagdAppendMenu( op_menu, "Options" );

  // register callback to handle all menus
  cagdRegisterCallback( CAGD_MENU, menu_callbacks, NULL );

  // register callback to handle lmb on cur_curve
  cagdRegisterCallback( CAGD_LBUTTONUP, left_mouse_click_cb, NULL );
}

/******************************************************************************
* myDialogProc REFINEMENT DIALOG
******************************************************************************/
LRESULT CALLBACK myDialogProc( HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam )
{
  if( message != WM_COMMAND )
    return FALSE;
  switch( LOWORD( wParam ) )
  {
  case IDOK:
    GetDlgItemText( hDialog, IDC_EDIT, myBuffer, sizeof( myBuffer ) );
    EndDialog( hDialog, TRUE );
    return TRUE;
  case IDCANCEL:
    EndDialog( hDialog, FALSE );
    return TRUE;
  default:
    return FALSE;
  }
}

/******************************************************************************
* myDialogProc2 ANIMATION SPEED DIALOG
******************************************************************************/
LRESULT CALLBACK myDialogProc2( HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam )
{
  if( message != WM_COMMAND )
    return FALSE;
  switch( LOWORD( wParam ) )
  {
  case IDOK:
    GetDlgItemText( hDialog, IDC_ANIM_SPEED_EDIT, myBuffer, sizeof( myBuffer ) );
    EndDialog( hDialog, TRUE );
    return TRUE;
  case IDCANCEL:
    EndDialog( hDialog, FALSE );
    return TRUE;
  default:
    return FALSE;
  }
}

/******************************************************************************
* offset_value_proc
******************************************************************************/
LRESULT CALLBACK offset_value_proc( HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam )
{
  if( message != WM_COMMAND )
    return FALSE;
  switch( LOWORD( wParam ) )
  {
  case IDOK:
    GetDlgItemText( hDialog, IDC_OFFSET_VALUE_EDIT, myBuffer, sizeof( myBuffer ) );
    EndDialog( hDialog, TRUE );
    return TRUE;
  case IDCANCEL:
    EndDialog( hDialog, FALSE );
    return TRUE;
  default:
    return FALSE;
  }
}

/******************************************************************************
* lower_domain_proc
******************************************************************************/
LRESULT CALLBACK lower_domain_proc( HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam )
{
  if( message != WM_COMMAND )
    return FALSE;
  switch( LOWORD( wParam ) )
  {
  case IDOK:
    GetDlgItemText( hDialog, IDC_LOWER_DOMAIN_EDIT, myBuffer, sizeof( myBuffer ) );
    EndDialog( hDialog, TRUE );
    return TRUE;
  case IDCANCEL:
    EndDialog( hDialog, FALSE );
    return TRUE;
  default:
    return FALSE;
  }
}

/******************************************************************************
* higher_domain_proc
******************************************************************************/
LRESULT CALLBACK higher_domain_proc( HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam )
{
  if( message != WM_COMMAND )
    return FALSE;
  switch( LOWORD( wParam ) )
  {
  case IDOK:
    GetDlgItemText( hDialog, IDC_HIGHER_DOMAIN_EDIT, myBuffer, sizeof( myBuffer ) );
    EndDialog( hDialog, TRUE );
    return TRUE;
  case IDCANCEL:
    EndDialog( hDialog, FALSE );
    return TRUE;
  default:
    return FALSE;
  }
}

/******************************************************************************
* x_parameter_proc
******************************************************************************/
LRESULT CALLBACK x_parameter_proc( HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam )
{
  if( message != WM_COMMAND )
    return FALSE;
  switch( LOWORD( wParam ) )
  {
  case IDOK:
    GetDlgItemText( hDialog, IDC_X_PARAMETER_EDIT, myBuffer, sizeof( myBuffer ) );
    EndDialog( hDialog, TRUE );
    return TRUE;
  case IDCANCEL:
    EndDialog( hDialog, FALSE );
    return TRUE;
  default:
    return FALSE;
  }
}

/******************************************************************************
* y_parameter_proc
******************************************************************************/
LRESULT CALLBACK y_parameter_proc( HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam )
{
  if( message != WM_COMMAND )
    return FALSE;
  switch( LOWORD( wParam ) )
  {
  case IDOK:
    GetDlgItemText( hDialog, IDC_Y_PARAMETER_EDIT, myBuffer, sizeof( myBuffer ) );
    EndDialog( hDialog, TRUE );
    return TRUE;
  case IDCANCEL:
    EndDialog( hDialog, FALSE );
    return TRUE;
  default:
    return FALSE;
  }
}

/******************************************************************************
* z_parameter_proc
******************************************************************************/
LRESULT CALLBACK z_parameter_proc( HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam )
{
  if( message != WM_COMMAND )
    return FALSE;
  switch( LOWORD( wParam ) )
  {
  case IDOK:
    GetDlgItemText( hDialog, IDC_Z_PARAMETER_EDIT, myBuffer, sizeof( myBuffer ) );
    EndDialog( hDialog, TRUE );
    return TRUE;
  case IDCANCEL:
    EndDialog( hDialog, FALSE );
    return TRUE;
  default:
    return FALSE;
  }
}

/******************************************************************************
* menu_callbacks
******************************************************************************/
void menu_callbacks( int id, int unUsed, PVOID userData )
{
  int is_error = 0;

  switch( id )
  {
  case CAGD_SEGS:
    handle_num_samples_menu();
    break;

  case CAGD_FRENET_ANIM_START:
    is_error = start_anim();
    break;

  case CAGD_FRENET_ANIM_STOP:
    if( frenet_anim_running == 1 )
    {
      stop_anim();
    }
    break;

  case CAGD_FRENET_ANIM_SPEED:
    handle_anim_speed_menu();
    break;

  case CAGD_OFFSET_CURVE_VALUE_MENU:
    handle_offset_curve_value_menu();
    break;

  case CAGD_LMB_FRENET_MENU:
    toggle_check_menu( g_lmb_menu, CAGD_LMB_FRENET_MENU );
    break;
  case CAGD_LMB_OSCULATING_MENU:
    toggle_check_menu( g_lmb_menu, CAGD_LMB_OSCULATING_MENU );
    break;
  case CAGD_LMB_TORSION_MENU:
    toggle_check_menu( g_lmb_menu, CAGD_LMB_TORSION_MENU );
    break;
  case CAGD_LMB_SPHERE_MENU:
    toggle_check_menu( g_lmb_menu, CAGD_LMB_SPHERE_MENU );
    break;

  case CAGD_ANIM_FRENET_MENU:
    toggle_check_menu( g_anim_settings_menu, CAGD_ANIM_FRENET_MENU );
    break;
  case CAGD_ANIM_OSCULATING_MENU:
    toggle_check_menu( g_anim_settings_menu, CAGD_ANIM_OSCULATING_MENU );
    break;
  case CAGD_ANIM_TORSION_MENU:
    toggle_check_menu( g_anim_settings_menu, CAGD_ANIM_TORSION_MENU );
    break;
  case CAGD_ANIM_SPHERE_MENU:
    toggle_check_menu( g_anim_settings_menu, CAGD_ANIM_SPHERE_MENU );
    break;

  case CAGD_SHOW_EVOLUTE_MENU:
    handle_evolute_check_menu();
    break;

  case CAGD_SHOW_OFFSET_CURVE_MENU:
    handle_offset_curve_check_menu();
    break;

  case CAGD_LOWER_DOMAIN_MENU:
    handle_minimum_domain_menu();
    break;

  case CAGD_HIGHER_DOMAIN_MENU:
    handle_maximum_domain_menu();
    break;

  case CAGD_X_PARAMETER_MENU:
    handle_x_parameter_menu();
    break;

  case CAGD_Y_PARAMETER_MENU:
    handle_y_parameter_menu();
    break;

  case CAGD_Z_PARAMETER_MENU:
    handle_z_parameter_menu();
    break;

  case CAGD_REDRAW_ALL:
    redraw_cb();
    break;
  }

  if( are_all_anim_menus_unchecked() )
    EnableMenuItem( g_animation_menu,
                    CAGD_FRENET_ANIM_START,
                    MF_GRAYED );
  else if( frenet_anim_running == 0 )
    EnableMenuItem( g_animation_menu,
                    CAGD_FRENET_ANIM_START,
                    MF_ENABLED );

}

/******************************************************************************
* left_mouse_click_cb
******************************************************************************/
void left_mouse_click_cb( int x, int y, PVOID userData )
{
  if( !cur_crv.defined )
  {
    return;
  }

  CAGD_POINT p = { 0.0,0.0,0.0 };
  cagdHideSegment( myText = cagdAddText( &p, "" ) );

  UINT id;
  int v;
  for( cagdPick( x, y ); id = cagdPickNext();)
    if( cagdGetSegmentType( id ) == CAGD_SEGMENT_POLYLINE )
      break;
  if( id && id == cur_crv.my_seg /* actually picked cur_crv and not other curve*/ )
  {
    if( v = cagdGetNearestVertex( id, x, y ) )
    {
      cagdGetVertex( id, --v, &p );
      double param = get_param_from_segment_number( v );

      free_all_segs( FALSE, FALSE );

      frenet_t frenet;
      calc_frenet( param, &frenet );

      if( frenet_anim_running == 1 )
      {
        stop_anim();
      }

      if( is_menu_checked( g_lmb_menu, CAGD_LMB_FRENET_MENU ) )
      {
        draw_frenet( param, &frenet );
      }

      if( is_menu_checked( g_lmb_menu, CAGD_LMB_OSCULATING_MENU ) )
      {
        draw_osc_circle( param, &frenet );
      }

      if( is_menu_checked( g_lmb_menu, CAGD_LMB_TORSION_MENU ) )
      {
        draw_helix( param, &frenet );
      }

      if( is_menu_checked( g_lmb_menu, CAGD_LMB_SPHERE_MENU ) )
      {
        draw_sphere( param, &frenet );
      }
    }
  }
  cagdRedraw();
}

/******************************************************************************
* handle_evolute_check_menu
******************************************************************************/
void handle_evolute_check_menu()
{
  toggle_check_menu( g_options_menu, CAGD_SHOW_EVOLUTE_MENU );

  if( is_show_evolute_menu_checked() )
  {
    if( cur_crv.defined )
    {
      set_evolute_color();
      draw_other_crv( num_samples * 3, NULL, &cur_crv.evolute_seg );

      set_default_color();
    }
  }
  else
  {
    cagdFreeSegment( cur_crv.evolute_seg );
    cur_crv.evolute_seg = K_NOT_USED;
    cagdRedraw();
  }
}

/******************************************************************************
* handle_offset_curve_check_menu
******************************************************************************/
void handle_offset_curve_check_menu()
{
  toggle_check_menu( g_offset_menu, CAGD_SHOW_OFFSET_CURVE_MENU );

  if( is_show_offset_curve_menu_checked() )
  {
    if( cur_crv.defined )
    {
      set_offset_color();
      draw_other_crv( num_samples * 2, &cur_crv.offset, &cur_crv.offset_seg );

      set_default_color();
    }
  }
  else
  {
    cagdFreeSegment( cur_crv.offset_seg );
    cur_crv.offset_seg = K_NOT_USED;
    cagdRedraw();
  }
}

/******************************************************************************
* handle_minimum_domain_menu
******************************************************************************/
void handle_minimum_domain_menu()
{
  stop_anim();

  if( DialogBox( cagdGetModule(),
      MAKEINTRESOURCE( IDD_LOWER_DOMAIN ),
      cagdGetWindow(),
      ( DLGPROC )lower_domain_proc ) )
  {
    double t = 0;

    if( sscanf( myBuffer, "%lf", &t ) == 1 )
    {
      cur_crv.domain[0] = t;
    }
  }
  else
  {
    myMessage( "Invalid number", "Please pick a valid number", MB_ICONERROR );
  }
}

/******************************************************************************
* handle_maximum_domain_menu
******************************************************************************/
void handle_maximum_domain_menu()
{
  stop_anim();

  if( DialogBox( cagdGetModule(),
      MAKEINTRESOURCE( IDD_HIGHER_DOMAIN ),
      cagdGetWindow(),
      ( DLGPROC )higher_domain_proc ) )
  {
    double t = 0;

    if( sscanf( myBuffer, "%lf", &t ) == 1 )
    {
      cur_crv.domain[1] = t;
    }
  }
  else
  {
    myMessage( "Invalid number", "Please pick a valid number", MB_ICONERROR );
  }
}

/******************************************************************************
* handle_x_parameter_menu
******************************************************************************/
void handle_x_parameter_menu()
{
  if( DialogBox( cagdGetModule(),
      MAKEINTRESOURCE( IDD_X_PARAMETER ),
      cagdGetWindow(),
      ( DLGPROC )x_parameter_proc ) )
  {
    char expression[BUFSIZ];

    if( sscanf( myBuffer, "%s", &expression ) == 1 )
      edit_cur_crv( 0, expression );
  }
  else
  {
    myMessage( "Invalid expression", "Please pick a valid expression", MB_ICONERROR );
  }
}

/******************************************************************************
* handle_y_parameter_menu
******************************************************************************/
void handle_y_parameter_menu()
{
  if( DialogBox( cagdGetModule(),
      MAKEINTRESOURCE( IDD_Y_PARAMETER ),
      cagdGetWindow(),
      ( DLGPROC )y_parameter_proc ) )
  {
    char expression[BUFSIZ];

    if( sscanf( myBuffer, "%s", &expression ) == 1 )
      edit_cur_crv( 1, expression );
  }
  else
  {
    myMessage( "Invalid expression", "Please pick a valid expression", MB_ICONERROR );
  }
}

/******************************************************************************
* handle_z_parameter_menu
******************************************************************************/
void handle_z_parameter_menu()
{
  if( DialogBox( cagdGetModule(),
      MAKEINTRESOURCE( IDD_Z_PARAMETER ),
      cagdGetWindow(),
      ( DLGPROC )z_parameter_proc ) )
  {
    char expression[BUFSIZ];

    if( sscanf( myBuffer, "%s", &expression ) == 1 )
      edit_cur_crv( 2, expression );
  }
  else
  {
    myMessage( "Invalid expression", "Please pick a valid expression", MB_ICONERROR );
  }
}

/******************************************************************************
* redraw_cb
******************************************************************************/
void redraw_cb()
{
  init_cur_crv();
}

/******************************************************************************
* handle_anim_speed_menu
******************************************************************************/
void handle_anim_speed_menu()
{
  if( DialogBox( cagdGetModule(),
      MAKEINTRESOURCE( IDC_ANIM_SPEED ),
      cagdGetWindow(),
      ( DLGPROC )myDialogProc2 ) )
  {
    double speed = 0;

    if( sscanf( myBuffer, "%lf", &speed ) == 1 )
    {
      double inv_speed = 1.0 / ( double )speed;
      frenet_anim_speed = inv_speed * 100.0;

      //TODO if have time. adjust anim speed values

      /*if( frenet_anim_speed == 1 )
      {
        int new_anim_smoothness = frenet_anim_smoothness - speed * 10;

        if( new_anim_smoothness < 200 )
          frenet_anim_smoothness = 200;
        else
          frenet_anim_smoothness = new_anim_smoothness;
      }
      else
        frenet_anim_smoothness = 2000;*/

      if( frenet_anim_running == 1 )
      {
        // apply new animation speed
        cagdRegisterCallback( CAGD_TIMER, NULL, NULL );
        cagdRegisterCallback( CAGD_TIMER, frenet_anim_cb, NULL );
      }
    }
    else
    {
      myMessage( "Invalid Speed", "Please pick a valid speed", MB_ICONERROR );
    }
  }
}

/******************************************************************************
* handle_offset_curve_value_menu
******************************************************************************/
void handle_offset_curve_value_menu()
{
  if( DialogBox( cagdGetModule(),
      MAKEINTRESOURCE( IDD_OFFSET_VALUE ),
      cagdGetWindow(),
      ( DLGPROC )offset_value_proc ) )
  {
    double offset_value;
    if( sscanf( myBuffer, "%lf", &offset_value ) == 1 )
    {
      cur_crv.offset = offset_value;

      if( is_show_offset_curve_menu_checked() )
      {
        set_offset_color();
        draw_other_crv( num_samples * 2, &cur_crv.offset, &cur_crv.offset_seg );

        set_default_color();
      }
    }
    else
    {
      myMessage( "Invalid Offset", "Please pick a valid offset", MB_ICONERROR );
    }
  }
}

/******************************************************************************
* handle_num_samples_menu
******************************************************************************/
void handle_num_samples_menu()
{
  if( DialogBox( cagdGetModule(),
      MAKEINTRESOURCE( IDD_COLOR ),
      cagdGetWindow(),
      ( DLGPROC )myDialogProc ) )
  {
    int new_samples = 0;

    if( sscanf( myBuffer, "%d", &new_samples ) == 1 && new_samples > 2 )
    {
      free_all_segs( TRUE, TRUE );
      num_samples = new_samples;

      if( cur_crv.defined == TRUE )
      {
        draw_cur_crv( num_samples );
      }
    }
    else
    {
      myMessage( "Invalid segments number", "invalid segs", MB_ICONERROR );
    }
  }
}

/******************************************************************************
* toggle_check_menu
******************************************************************************/
void toggle_check_menu( HMENU main_menu, UINT sub_menu_id )
{
  if( GetMenuState( main_menu, sub_menu_id, MF_BYCOMMAND ) & MF_CHECKED )
    CheckMenuItem( main_menu, sub_menu_id, MF_UNCHECKED );
  else
    CheckMenuItem( main_menu, sub_menu_id, MF_CHECKED );
}

/******************************************************************************
* is_menu_checked
******************************************************************************/
int is_menu_checked( HMENU main_menu, UINT sub_menu_id )
{
  return ( GetMenuState( main_menu, sub_menu_id, MF_BYCOMMAND ) & MF_CHECKED );
}

/******************************************************************************
* show_evolute_menu_checked
******************************************************************************/
int is_show_evolute_menu_checked()
{
  return is_menu_checked( g_options_menu, CAGD_SHOW_EVOLUTE_MENU );
}

/******************************************************************************
* is_show_offset_curve_menu_checked
******************************************************************************/
int is_show_offset_curve_menu_checked()
{
  return is_menu_checked( g_offset_menu, CAGD_SHOW_OFFSET_CURVE_MENU );
}

/******************************************************************************
* are_all_anim_menus_unchecked
******************************************************************************/
int are_all_anim_menus_unchecked()
{
  if( !is_menu_checked( g_anim_settings_menu, CAGD_ANIM_FRENET_MENU ) &&
      !is_menu_checked( g_anim_settings_menu, CAGD_ANIM_OSCULATING_MENU ) &&
      !is_menu_checked( g_anim_settings_menu, CAGD_ANIM_TORSION_MENU ) &&
      !is_menu_checked( g_anim_settings_menu, CAGD_ANIM_SPHERE_MENU ) )
    return TRUE;
  else
    return FALSE;
}