#include <cagd.h>
#include <stdio.h>
#include "menus.h"
#include "resource.h"
#include "frenet.h"
#include "evolute_n_offset.h"
#include "color.h"
#include "crvtr.h"
#include "trsn.h"

char myBuffer[BUFSIZ];
UINT myText;
HMENU g_lmb_menu = NULL;
HMENU g_options_menu = NULL;

extern HMENU g_anim_settings_menu;
extern int num_samples;
extern int frenet_anim_running;
extern int frenet_anim_speed;
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

  g_lmb_menu = lmb_menu;
  g_anim_settings_menu = anim_settings_menu;
  g_options_menu = op_menu;

  // options
  AppendMenu( op_menu, MF_STRING, CAGD_SEGS, "Change Refinement" );
  AppendMenu( op_menu, MF_STRING, CAGD_SHOW_EVOLUTE_MENU, "Show Evolute Curve" );
  AppendMenu( op_menu, MF_STRING | MF_POPUP, ( UINT_PTR )lmb_menu, "Left Mouse button" );
  AppendMenu( op_menu, MF_STRING | MF_POPUP, ( UINT_PTR )anim_settings_menu, "Animation Settings" );

  // frenet
  AppendMenu( fre_menu, MF_STRING, CAGD_FRENET_ANIM_START, "Start Animation" );
  AppendMenu( fre_menu, MF_STRING, CAGD_FRENET_ANIM_STOP, "Stop Animation" );

  // lmb sub menu
  AppendMenu( lmb_menu, MF_STRING | MF_CHECKED, CAGD_LMB_FRENET_MENU, "Show Frenet Frame" );
  AppendMenu( lmb_menu, MF_STRING, CAGD_LMB_OSCULATING_MENU, "Show Osculating Circle" );
  AppendMenu( lmb_menu, MF_STRING, CAGD_LMB_TORSION_MENU, "Show Torsion Helix" );

  // animation settings menu
  AppendMenu( anim_settings_menu, MF_STRING | MF_CHECKED, CAGD_ANIM_FRENET_MENU, "Animate Frenet Frame" );
  AppendMenu( anim_settings_menu, MF_STRING, CAGD_ANIM_OSCULATING_MENU, "Animate Osculating Circle" );
  AppendMenu( anim_settings_menu, MF_STRING, CAGD_ANIM_TORSION_MENU , "Animate Torsion Helix" );
  AppendMenu( anim_settings_menu, MF_SEPARATOR, 0, NULL );
  AppendMenu( anim_settings_menu, MF_STRING, CAGD_FRENET_ANIM_SPEED, "Set Animation Speed" );

  // adding to cagd
  cagdAppendMenu( fre_menu, "Animation" );
  cagdAppendMenu( op_menu, "Options" );

  // register callback to handle all menus
  cagdRegisterCallback( CAGD_MENU, menu_callbacks, NULL );

  // register callback to handle lmb on cur_curve
  cagdRegisterCallback( CAGD_LBUTTONUP, left_mouse_click_cb, NULL );
}

/******************************************************************************
* myDialogProc
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
* myDialogProc2
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
    is_error = frenet_start_animation();
    break;

  case CAGD_FRENET_ANIM_STOP:
    if( frenet_anim_running == 1 )
    {
      stop_frenet_animation();
    }
    break;

  case CAGD_FRENET_ANIM_SPEED:
    handle_anim_speed_menu();
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

  case CAGD_ANIM_FRENET_MENU:
    toggle_check_menu( g_anim_settings_menu, CAGD_ANIM_FRENET_MENU );
    break;
  case CAGD_ANIM_OSCULATING_MENU:
    toggle_check_menu( g_anim_settings_menu, CAGD_ANIM_OSCULATING_MENU );
    break;
  case CAGD_ANIM_TORSION_MENU:
    toggle_check_menu( g_anim_settings_menu, CAGD_ANIM_TORSION_MENU );
    break;

  case CAGD_SHOW_EVOLUTE_MENU:
    handle_evolute_check_menu();
    break;
  }
}

/******************************************************************************
* left_mouse_click_cb
******************************************************************************/
void left_mouse_click_cb( int x, int y, PVOID userData )
{
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

      free_all_segs( FALSE );

      frenet_t frenet;
      calc_frenet( param, &frenet );

      if( frenet_anim_running == 1 )
      {
        stop_frenet_animation();
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
    set_evolute_color();
    draw_other_crv( num_samples * 2, NULL, &cur_crv.evolute_seg );

    set_default_color();
  }
  else
  {
    cagdFreeSegment( cur_crv.evolute_seg );
    cur_crv.evolute_seg = K_NOT_USED;
    cagdRedraw();
  }
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
    int speed = 0;

    if( sscanf( myBuffer, "%d", &speed ) == 1 )
    {
      double inv_speed = 1 / ( double )speed;
      frenet_anim_speed = ( int )( inv_speed * 100 );

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
      cagdFreeAllSegments();
      num_samples = new_samples;
      draw_cur_crv( num_samples );
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