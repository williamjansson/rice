--- config.def.h.orig	2020-10-31 00:35:50.000000000 +0900
+++ config.def.h	2020-11-19 22:30:27.776386732 +0900
@@ -1,14 +1,14 @@
 /* See LICENSE file for copyright and license details. */
-#define CWD   "cwd: "
-#define CURSR " > "
+#define CWD   "   "
+#define CURSR ">  "
 #define EMPTY "   "
 
-int dirorder    = 0; /* Set to 1 to sort by directory first */
-int mtimeorder  = 0; /* Set to 1 to sort by time modified */
+int dirorder    = 1; /* Set to 1 to sort by directory first */
+int mtimeorder  = 1; /* Set to 1 to sort by time modified */
 int icaseorder  = 0; /* Set to 1 to sort by ignoring case */
 int idletimeout = 0; /* Screensaver timeout in seconds, 0 to disable */
 int showhidden  = 0; /* Set to 1 to show hidden files by default */
-int usecolor    = 0; /* Set to 1 to enable color attributes */
+int usecolor    = 1; /* Set to 1 to enable color attributes */
 char *idlecmd   = "rain"; /* The screensaver program */
 
 /* See curs_attr(3) for valid video attributes */
@@ -33,8 +33,8 @@
 
 struct assoc assocs[] = {
 	{ "\\.(avi|mp4|mkv|mp3|ogg|flac|mov)$", "mpv" },
-	{ "\\.(png|jpg|gif)$", "sxiv" },
-	{ "\\.(html|svg)$", "firefox" },
+	{ "\\.(png|jpg|jpeg|gif)$", "sxiv" },
+	{ "\\.(svg)$", "firefox" },
 	{ "\\.pdf$", "mupdf" },
 	{ "\\.sh$", "sh" },
 	{ ".", "less" },
@@ -88,12 +88,14 @@
 	/* Toggle sort by time */
 	{ 't',            SEL_MTIME },
 	/* Toggle case sensitivity */
-	{ 'i',            SEL_ICASE },
+	{ 'v',            SEL_ICASE },
 	{ CONTROL('L'),   SEL_REDRAW },
 	/* Run command */
 	{ 'z',            SEL_RUN, "top" },
-	{ '!',            SEL_RUN, "sh", "SHELL" },
+	{ 'i',            SEL_RUN, "img" },
+	{ '!',            SEL_RUN, "termcmd" },
 	/* Run command with argument */
-	{ 'e',            SEL_RUNARG, "vi", "EDITOR" },
-	{ 'p',            SEL_RUNARG, "less", "PAGER" },
+	{ 'e',            SEL_RUNARG, "edit" },
+	{ 'x',            SEL_RUNARG, "sh" },
+	{ 'p',            SEL_RUNARG, "play" },
 };
