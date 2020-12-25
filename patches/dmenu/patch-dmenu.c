--- dmenu.c.orig	2020-11-01 21:33:19.133343937 +0900
+++ dmenu.c	2020-11-01 21:41:14.363040151 +0900
@@ -37,6 +37,9 @@
 static char text[BUFSIZ] = "";
 static char *embed;
 static int bh, mw, mh;
+static int dmx = 0; /* put dmenu at this x offset */
+static int dmy = 0; /* put dmenu at this y offset (measured from the bottom if topbar is 0) */
+static unsigned int dmw = 0; /* make dmenu this wide */
 static int inputw = 0, promptw;
 static int lrpad; /* sum of left and right padding */
 static size_t cursor;
@@ -637,9 +640,9 @@
 				if (INTERSECT(x, y, 1, 1, info[i]))
 					break;
 
-		x = info[i].x_org;
-		y = info[i].y_org + (topbar ? 0 : info[i].height - mh);
-		mw = info[i].width;
+		x = info[i].x_org + dmx;
+		y = info[i].y_org + (topbar ? dmy : info[i].height - mh - dmy);
+		mw = (dmw>0 ? dmw : info[i].width);
 		XFree(info);
 	} else
 #endif
@@ -647,9 +650,9 @@
 		if (!XGetWindowAttributes(dpy, parentwin, &wa))
 			die("could not get embedding window attributes: 0x%lx",
 			    parentwin);
-		x = 0;
-		y = topbar ? 0 : wa.height - mh;
-		mw = wa.width;
+		x = dmx;
+		y = topbar ? dmy : wa.height - mh - dmy;
+		mw = (dmw>0 ? dmw : wa.width);
 	}
 	promptw = (prompt && *prompt) ? TEXTW(prompt) - lrpad / 4 : 0;
 	inputw = MIN(inputw, mw/3);
@@ -690,6 +693,7 @@
 usage(void)
 {
 	fputs("usage: dmenu [-bfiv] [-l lines] [-p prompt] [-fn font] [-m monitor]\n"
+	      "             [-h height] [-x xoffset] [-y yoffset] [-w width]\n"
 	      "             [-nb color] [-nf color] [-sb color] [-sf color] [-w windowid]\n", stderr);
 	exit(1);
 }
@@ -717,6 +721,12 @@
 		/* these options take one argument */
 		else if (!strcmp(argv[i], "-l"))   /* number of lines in vertical list */
 			lines = atoi(argv[++i]);
+		else if (!strcmp(argv[i], "-x"))   /* window x offset */
+			dmx = atoi(argv[++i]);
+		else if (!strcmp(argv[i], "-y"))   /* window y offset (from bottom up if -b) */
+			dmy = atoi(argv[++i]);
+		else if (!strcmp(argv[i], "-w"))   /* make dmenu this wide */
+			dmw = atoi(argv[++i]);
 		else if (!strcmp(argv[i], "-m"))
 			mon = atoi(argv[++i]);
 		else if (!strcmp(argv[i], "-p"))   /* adds prompt to left of input field */
