--- noice.c.orig	2020-10-14 14:48:22.784770457 +0000
+++ noice.c	2020-10-20 10:38:27.573847760 +0000
@@ -474,10 +474,10 @@
 		len = strlen(name) + 1;
 
 	if (S_ISDIR(ent->mode)) {
-		cm = '/';
+		cm = '\0';
 		attr |= DIR_ATTR;
 	} else if (S_ISLNK(ent->mode)) {
-		cm = '@';
+		cm = '\0';
 		attr |= LINK_ATTR;
 	} else if (S_ISSOCK(ent->mode)) {
 		cm = '=';
