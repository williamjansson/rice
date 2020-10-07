--- config.def.h.orig	2020-08-31 07:59:27.979014427 +0000
+++ config.def.h	2020-08-31 08:03:34.300005822 +0000
@@ -4,17 +4,17 @@
 static int topbar = 1;                      /* -b  option; if 0, dmenu appears at bottom     */
 /* -fn option overrides fonts[0]; default X11 font or font set */
 static const char *fonts[] = {
-	"monospace:size=10"
+	"Fixed:style=semicondensed:pixelsize=13:lang=en"
 };
 static const char *prompt      = NULL;      /* -p  option; prompt to the left of input field */
 static const char *colors[SchemeLast][2] = {
 	/*     fg         bg       */
-	[SchemeNorm] = { "#bbbbbb", "#222222" },
-	[SchemeSel] = { "#eeeeee", "#005577" },
+	[SchemeNorm] = { "#1e2d1c", "#f0eed8" },
+	[SchemeSel] = { "#f0eed8", "#1e2d1c" },
 	[SchemeOut] = { "#000000", "#00ffff" },
 };
 /* -l option; if nonzero, dmenu uses vertical list with given number of lines */
-static unsigned int lines      = 0;
+static unsigned int lines      = 9;
 
 /*
  * Characters not considered part of a word while deleting words
