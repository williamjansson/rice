export LANG=ja_JP.UTF-8
export LC_ALL=ja_JP.UTF-8
export LC_CTYPE=ja_JP.UTF-8
setxkbmap -model jp106 -layout jp

export XMODIFIERS=@im=uim

xsetroot -solid "#faebd7"

xrdb ~/.Xresources

#xset -dpms
xset s off

#uim-xim &
#uim-toolbar-gtk &

#sleep 1 && unclutter -root -idle 1 &

exec dwm
