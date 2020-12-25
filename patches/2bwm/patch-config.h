--- config.h.orig	2020-11-07 17:07:51.190840349 +0900
+++ config.h	2020-12-25 17:15:01.172181849 +0900
@@ -19,7 +19,7 @@
  *2)fixedcol         3)unkilcol
  *4)fixedunkilcol    5)outerbordercol
  *6)emptycol         */
-static const char *colors[] = {"#35586c","#333333","#7a8c5c","#ff6666","#cc9933","#0d131a","#000000"};
+static const char *colors[] = {"#35586c","#333333","#7a8c5c","#ff6666","#cc9933","#0d131a","#f0eed8"};
 /* if this is set to true the inner border and outer borders colors will be swapped */
 static const bool inverted_colors = true;
 ///---Cursor---///
@@ -32,7 +32,7 @@
 /*0) Outer border size. If you put this negative it will be a square.
  *1) Full borderwidth    2) Magnet border size
  *3) Resize border size  */
-static const uint8_t borders[] = {3,5,5,4};
+static const uint8_t borders[] = {0,0,0,0};
 /* Windows that won't have a border.
  * It uses substring comparison with what is found in the WM_NAME
  * attribute of the window. You can test this using `xprop WM_NAME`
@@ -40,7 +40,13 @@
 #define LOOK_INTO "WM_NAME"
 static const char *ignore_names[] = {"bar", "xclock"};
 ///--Menus and Programs---///
-static const char *menucmd[]   = { "", NULL };
+static const char *menucmd[]   = { "menucmd", NULL };
+static const char *termcmd[]   = { "termcmd", NULL };
+static const char *filecmd[]   = { "filecmd", NULL };
+static const char *clickcmd[]   = { "clickcmd", NULL };
+static const char *volup[]   = { "volup", NULL };
+static const char *voldown[]   = { "voldown", NULL };
+static const char *mute[]   = { "mute", NULL };
 ///--Custom foo---///
 static void halfandcentered(const Arg *arg)
 {
@@ -84,7 +90,7 @@
     {  MOD ,              XK_Tab,        focusnext,         {.i=TWOBWM_FOCUS_NEXT}},
     {  MOD |SHIFT,        XK_Tab,        focusnext,         {.i=TWOBWM_FOCUS_PREVIOUS}},
     // Kill a window
-    {  MOD ,              XK_q,          deletewin,         {}},
+    {  MOD ,              XK_w,          deletewin,         {}},
     // Resize a window
     {  MOD |SHIFT,        XK_k,          resizestep,        {.i=TWOBWM_RESIZE_UP}},
     {  MOD |SHIFT,        XK_j,          resizestep,        {.i=TWOBWM_RESIZE_DOWN}},
@@ -124,7 +130,7 @@
     {  MOD ,              XK_Home,       resizestep_aspect, {.i=TWOBWM_RESIZE_KEEP_ASPECT_GROW}},
     {  MOD ,              XK_End,        resizestep_aspect, {.i=TWOBWM_RESIZE_KEEP_ASPECT_SHRINK}},
     // Maximize (ignore offset and no EWMH atom)
-    {  MOD ,              XK_x,          maximize,          {}},
+    {  MOD ,              XK_f,          maximize,          {}},
     // Full screen (disregarding offsets and adding EWMH atom)
     {  MOD |SHIFT ,       XK_x,          fullscreen,        {}},
     // Maximize vertically
@@ -166,7 +172,7 @@
     // Make the window appear always on top
     {  MOD,               XK_t,          always_on_top,     {}},
     // Make the window stay on all workspaces
-    {  MOD ,              XK_f,          fix,               {}},
+    {  MOD |ALT,          XK_f,          fix,               {}},
     // Move the cursor
     {  MOD ,              XK_Up,         cursor_move,       {.i=TWOBWM_CURSOR_UP_SLOW}},
     {  MOD ,              XK_Down,       cursor_move,       {.i=TWOBWM_CURSOR_DOWN_SLOW}},
@@ -178,11 +184,17 @@
     {  MOD |SHIFT,        XK_Right,      cursor_move,       {.i=TWOBWM_CURSOR_RIGHT}},
     {  MOD |SHIFT,        XK_Left,       cursor_move,       {.i=TWOBWM_CURSOR_LEFT}},
     // Start programs
-    {  MOD ,              XK_w,          start,             {.com = menucmd}},
+    {  MOD ,              XK_space,      start,             {.com = menucmd}},
+    {  MOD ,              XK_Return,     start,             {.com = termcmd}},
+    {  MOD |SHIFT,        XK_Return,     start,             {.com = clickcmd}},
+    {  MOD ,              XK_x,          start,             {.com = filecmd}},
+    {  0,                 XK_F6,         start,             {.com = volup}},
+    {  0,                 XK_F5,         start,             {.com = voldown}},
+    {  0,                 XK_F4,         start,             {.com = mute}},
     // Exit or restart 2bwm
     {  MOD |CONTROL,      XK_q,          twobwm_exit,       {.i=0}},
     {  MOD |CONTROL,      XK_r,          twobwm_restart,    {.i=0}},
-    {  MOD ,              XK_space,      halfandcentered,   {.i=0}},
+    {  MOD |CONTROL,      XK_space,      halfandcentered,   {.i=0}},
     // Change current workspace
        DESKTOPCHANGE(     XK_1,                             0)
        DESKTOPCHANGE(     XK_2,                             1)
