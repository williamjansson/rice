--- src/feature.h.orig	2020-03-16 05:44:41.773044407 +0000
+++ src/feature.h	2020-03-16 05:47:54.400418622 +0000
@@ -71,7 +71,7 @@
  * overstrike just for multi char fonts
  * Note: NO_BOLDOVERSTRIKE implies NO_BOLDOVERSTRIKE_MULTI
  */
-/* #define NO_BOLDOVERSTRIKE */
+#define NO_BOLDOVERSTRIKE
 /* #define NO_BOLDOVERSTRIKE_MULTI */
 
 /*
@@ -83,12 +83,12 @@
 /*
  * Also use bold font or overstrike even if we use colour for bold
  */
-#define VERYBOLD
+/* #define VERYBOLD */
 
 /*
  * Compile without support for real bold fonts
  */
-/* #define NO_BOLDFONT */
+#define NO_BOLDFONT
 
 /*
  * Add support for '-hc colour' for background highlight of selection
