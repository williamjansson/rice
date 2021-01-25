export LANG=ja_JP.UTF-8
export LC_ALL=ja_JP.UTF-8
export LC_CTYPE=ja_JP.UTF-8

export GTK_IM_MODULE=uim
export XMODIFIERS=@im=uim

setxkbmap -model jp106 -layout jp

xsetroot -solid "#f0eed8"
xsetroot -cursor_name left_ptr
xrdb ~/.config/.Xresources
xset s off
xset -dpms
#urxvtd -q -o -f &

xset fp+ /usr/X11R7/lib/X11/fonts/custom
xset fp rehash
filecmd

#GTK_THEME=Arc uim-xim &

xdotool mousemove 1920 1080

exec sowm
