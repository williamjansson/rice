export LANG=ja_JP.eucJP
export LC_ALL=ja_JP.eucJP
export LC_CTYPE=ja_JP.eucJP
setxkbmap -model jp106 -layout jp

export XMODIFIERS=@im=uim

xsetroot -solid "#f0f0dc"

xrdb ~/.Xresources

#xset -dpms
xset s off

xset fp+ /usr/pkg/share/fonts/X11/local
xset fp rehash

#uim-xim &
#uim-toolbar-gtk &

exec dwm
