#include <cagd.h>
#include <stdio.h>
#include "resource.h"
#include "cur_crv.h"
#include "frenet.h"
#include "menus.h"

extern int frenet_anim_speed;
extern int frenet_anim_running;
extern int num_samples;

#if defined(_WIN32)
    #if _MSC_VER >= 1900
  #pragma comment(lib, "legacy_stdio_definitions.lib")
    #endif
#endif

enum {
  MY_CLICK = CAGD_USER,
  MY_POLY,
  MY_ANIM,
  MY_DRAG,
  MY_ADD,
  MY_COLOR,
  MY_REMOVE,
};

char *animText[] = {
  "Animation Demo",
  "During the animation you can freely\n"
  "rotate, translate and scale the scene."
};

char *dragText[] = {
  "Drag, Popups & Dialog Demo",
  "Click right mouse button to call popup menu.\n"
  "A contents of the menu depends on where you\n"
  "clicked. Clicking on an existing point allows\n"
  "you to remove the point or change its color via\n"
  "dialog. If there is no point you could add one.\n\n"
  "Using left mouse button you can drag an existing\n"
  "points. Watch carefully a text appearing near\n"
  "the point being draged and don't miss."
};

char *clickText[] = {
  "Click Demo",
  "CAGD unable to convert 2D point defined by your\n"
  "click to single 3D point located in the object\n"
  "space. Instead it returns you two 3D points\n"
  "as specification of vector. Initially you are\n"
  "looking in the direction of this vector and cannot\n"
  "see anything. Try to rotate the scene after few\n"
  "clicks. You'll see polylines defined by the returned\n"
  "vectors."
};

char *polyText[] = {
  "Polyline Demo",
  "Click polyline. The nearest vertex will be marked with\n"
  "text. Remember that \"nearest\" defined by the minimal\n"
  "distance on the screen (window coordinates) and not\n"
  "in the object space."
};

HMENU myPopup;
UINT myText;
char myBuffer[BUFSIZ];

void myMessage(PSTR title, PSTR message, UINT type)
{
  MessageBox(cagdGetWindow(), message, title, MB_OK | MB_APPLMODAL | type);
}

void myTimer(int x, int y, PVOID userData)
{
  cagdRotate(2, 0, 1, 0);
  cagdRedraw();
}

int main(int argc, char *argv[])
{
  cagdBegin( "CAGD", 800, 800 );
  init_menus();

  num_samples = NUM_SAMPS; // Default value
  init_all_segs();

  cagdShowHelp();
  cagdMainLoop();

  return 0;
}
