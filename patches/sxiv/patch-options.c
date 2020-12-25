--- options.c.orig	2020-10-14 08:30:59.010635623 +0000
+++ options.c	2020-10-14 08:31:17.579787858 +0000
@@ -63,7 +63,7 @@
 
 	_options.fullscreen = false;
 	_options.embed = 0;
-	_options.hide_bar = false;
+	_options.hide_bar = true;
 	_options.geometry = NULL;
 	_options.res_name = NULL;
 
@@ -87,7 +87,7 @@
 				_options.animate = true;
 				break;
 			case 'b':
-				_options.hide_bar = true;
+				_options.hide_bar = false;
 				break;
 			case 'c':
 				_options.clean_cache = true;
