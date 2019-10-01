/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 0;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     iscentered     isfloating   monitor */
	{ "tabbed",   NULL,       NULL,       0,            0,             0,           -1 },
	{ "st",       NULL,       NULL,       0,            1,             1,           -1 },
	{ "Emacs",    NULL,       NULL,       0,            1,             1,           -1 },
	{ "Gpick",    NULL,       NULL,       0,            1,             1,           -1 },
    { "Thunar",   NULL,       NULL,       0,            1,             1,           -1 },
    { "feh",      NULL,       NULL,       0,            1,             1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *emacs[] = { "emacs", NULL };
static const char *terminal[] = { "emacsclient", "-cn", "-F", "((width . 48) (height . 33))", "-e", "(eshell)", "x", NULL };
static const char *editor[] = { "tabbed", "-dc", "emacsclient", "-n", "~/.startemx", "--parent-id", NULL };
static const char *files[] = { "emacsclient", "-cn", "-F", "((width . 37) (height . 30))", "-e", "(ranger)", "x", NULL };
static const char *launcher[] = { "rofi_dmenu", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|Mod1Mask,              XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|Mod1Mask,              XK_x,      spawn,          {.v = emacs } },
    { MODKEY,                       XK_z,      spawn,          {.v = files } },
	{ MODKEY,                       XK_Return, spawn,          {.v = terminal } },
	{ MODKEY,                       XK_x,      spawn,          {.v = editor } },
	{ MODKEY,                       XK_space,  spawn,          {.v = launcher } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_z,      zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_w,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_space,  setlayout,      {0} },
	{ MODKEY|Mod1Mask,              XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
    { MODKEY|ShiftMask,             XK_Up,     spawn,          SHCMD("exec xdotool mousemove_relative -- 0 -15")},
    { MODKEY|ShiftMask,             XK_Down,   spawn,          SHCMD("exec xdotool mousemove_relative -- 0 15")},
    { MODKEY|ShiftMask,             XK_Right,  spawn,          SHCMD("exec xdotool mousemove_relative -- 15 0")},
    { MODKEY|ShiftMask,             XK_Left,   spawn,          SHCMD("exec xdotool mousemove_relative -- -15 0")},
    { MODKEY|ShiftMask,             XK_Return, spawn,          SHCMD("exec xdotool click 1")},
    { MODKEY|ShiftMask,             XK_o,      spawn,          SHCMD("exec xdotool click 2")},
    { MODKEY|ShiftMask,             XK_p,      spawn,          SHCMD("exec xdotool click 3")},
    { MODKEY|ShiftMask,             XK_d,      spawn,          SHCMD("exec xdotool mousedown 1")},
    { MODKEY|ShiftMask,             XK_u,      spawn,          SHCMD("exec xdotool mouseup 1")},
    { MODKEY,                       XK_Down,   moveresize,     {.v = (int []){ 0, 25, 0, 0 }}},
    { MODKEY,                       XK_Up,     moveresize,     {.v = (int []){ 0, -25, 0, 0 }}},
    { MODKEY,                       XK_Right,  moveresize,     {.v = (int []){ 25, 0, 0, 0 }}},
    { MODKEY,                       XK_Left,   moveresize,     {.v = (int []){ -25, 0, 0, 0 }}},
    { MODKEY|Mod1Mask,              XK_Down,   moveresize,     {.v = (int []){ 0, 0, 0, 25 }}},
    { MODKEY|Mod1Mask,              XK_Up,     moveresize,     {.v = (int []){ 0, 0, 0, -25 }}},
    { MODKEY|Mod1Mask,              XK_Right,  moveresize,     {.v = (int []){ 0, 0, 25, 0 }}},
    { MODKEY|Mod1Mask,              XK_Left,   moveresize,     {.v = (int []){ 0, 0, -25, 0 }}},
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|Mod1Mask,              XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
