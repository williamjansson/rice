--- config.def.h.orig	2020-10-31 00:35:50.000000000 +0900
+++ config.def.h	2020-11-05 21:21:27.456767761 +0900
@@ -48,7 +48,7 @@
 #ifdef _THUMBS_CONFIG
 
 /* thumbnail sizes in pixels (width == height): */
-static const int thumb_sizes[] = { 32, 64, 96, 128, 160 };
+static const int thumb_sizes[] = { 32, 64, 96, 128, 160, 220, 400 };
 
 /* thumbnail size at startup, index into thumb_sizes[]: */
 static const int THUMB_SIZE = 3;
