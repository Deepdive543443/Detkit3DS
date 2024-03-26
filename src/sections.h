#ifndef SECTIONS_H
#define SECTIONS_H
#include <setjmp.h>
#include "3ds.h"
#include "detector.h"
#include "lvgl.h"

#define WIDTH_BTM   320
#define HEIGHT_BTM  240
#define WIDTH_TOP   400
#define HEIGHT_TOP  240
#define SCRSIZE_TOP WIDTH_TOP * HEIGHT_TOP

#define STACKSIZE    (4 * 1024)
#define TICK_MS      1               // Millisec a tick
#define TICK_US      TICK_MS * 1000  // Microsec a tick
#define TICK_NS      TICK_US * 1000  // Nanosec a tick
#define USE_SYS_CORE 0               /*Sys code leads to slower loading speed and it's not available in CIA*/

#include "sections/cam.h"
#include "sections/display.h"
#include "sections/input.h"
#include "sections/thread.h"
#include "sections/ui.h"
#include "sections/widgets/widget.h"

// Glob
extern jmp_buf  g_exitJmp;
extern Detector g_det;
#endif  // SECTIONS_H