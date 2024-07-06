#include <cagd.h>
#include <stdio.h>
#include "menus.h"
#include "resource.h"
#include "frenet.h"

char myBuffer[BUFSIZ];
UINT myText;
HMENU g_lmb_menu = NULL;

extern int num_samples;
extern int frenet_anim_running;
extern int frenet_anim_speed;

void init_menus()
{
  HMENU op_menu = CreatePopupMenu(); // options
  HMENU fre_menu = CreatePopupMenu(); // frenet
  HMENU lmb_menu = CreatePopupMenu(); // lmb sub menu

  g_lmb_menu = lmb_menu;

  // options
  AppendMenu( op_menu, MF_STRING, CAGD_SEGS, "Refinement" );
  AppendMenu( op_menu, MF_STRING, CAGD_FRENET_ANIM_SPEED, "Animation Speed" );
  AppendMenu( op_menu, MF_STRING | MF_POPUP, ( UINT_PTR )lmb_menu, "Left Mouse button" );

  // frenet
  AppendMenu( fre_menu, MF_STRING, CAGD_FRENET_ANIM_START, "Start Frenet Animation" );
  AppendMenu( fre_menu, MF_STRING, CAGD_FRENET_ANIM_STOP, "Stop Frenet Animation" );

  // lmb sub menu
  AppendMenu( lmb_menu, MF_STRING | MF_CHECKED, CAGD_LMB_FRENET, "Show Frenet Frame" );
  AppendMenu( lmb_menu, MF_STRING, CAGD_LMB_OSCULATING, "Show Osculating Circle" );
  AppendMenu( lmb_menu, MF_STRING, CAGD_LMB_TORSION, "Show Torsion Helix" );

  // adding to cagd
  cagdAppendMenu( fre_menu, "Frenet" );
  cagdAppendMenu( op_menu, "Options" );
  cagdRegisterCallback( CAGD_MENU, menu_callbacks, NULL );

  cagdRegisterCallback( CAGD_LBUTTONUP, left_mouse_click_cb, NULL );
}

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

void menu_callbacks( int id, int unUsed, PVOID userData )
{
  int is_error = 0;

  switch( id )
  {
  case CAGD_SEGS:
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
    break;

  case CAGD_FRENET_ANIM_START:
    if( cur_crv.defined == 0 )
    {
      print_err( "Please Load a Curve first" );
      is_error = 1;
    }
    if( is_error == 0 )
    {
      frenet_anim_running = 1;
      cagdRegisterCallback( CAGD_TIMER, frenet_anim_cb, NULL );
    }
    break;

  case CAGD_FRENET_ANIM_STOP:
    if( frenet_anim_running == 1 )
    {
      stop_frenet_animation();
    }
    break;

  case CAGD_FRENET_ANIM_SPEED:
    if( DialogBox( cagdGetModule(),
        MAKEINTRESOURCE( IDC_ANIM_SPEED ),
        cagdGetWindow(),
        ( DLGPROC )myDialogProc2 ) )
    {
      int speed = 0;

      if( sscanf( myBuffer, "%d", &speed ) == 1 )
      {
        double inv_speed = 1 / ( double )speed;
        frenet_anim_speed = ( inv_speed ) * 100;

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
    break;

  case CAGD_LMB_FRENET:
    if( GetMenuState( g_lmb_menu, CAGD_LMB_FRENET, MF_BYCOMMAND ) & MF_CHECKED )
      CheckMenuItem( g_lmb_menu, CAGD_LMB_FRENET, MF_UNCHECKED );
    else
      CheckMenuItem( g_lmb_menu, CAGD_LMB_FRENET, MF_CHECKED );
    break;
  case CAGD_LMB_OSCULATING:
    if( GetMenuState( g_lmb_menu, CAGD_LMB_OSCULATING, MF_BYCOMMAND ) & MF_CHECKED )
      CheckMenuItem( g_lmb_menu, CAGD_LMB_OSCULATING, MF_UNCHECKED );
    else
      CheckMenuItem( g_lmb_menu, CAGD_LMB_OSCULATING, MF_CHECKED );
    break;
  case CAGD_LMB_TORSION:
    if( GetMenuState( g_lmb_menu, CAGD_LMB_TORSION, MF_BYCOMMAND ) & MF_CHECKED )
      CheckMenuItem( g_lmb_menu, CAGD_LMB_TORSION, MF_UNCHECKED );
    else
      CheckMenuItem( g_lmb_menu, CAGD_LMB_TORSION, MF_CHECKED );
    break;
  }
}

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

      if( GetMenuState( g_lmb_menu, CAGD_LMB_FRENET, MF_BYCOMMAND ) & MF_CHECKED )
      {
        draw_frenet( param, &frenet );
      }

      if( GetMenuState( g_lmb_menu, CAGD_LMB_OSCULATING, MF_BYCOMMAND ) & MF_CHECKED )
      {
        draw_osc_circle( param, &frenet );
      }

      if( GetMenuState( g_lmb_menu, CAGD_LMB_TORSION, MF_BYCOMMAND ) & MF_CHECKED )
      {
        draw_helix( param, &frenet );
      }
    }
  }
  cagdRedraw();
}