export LANG=ja_JP.eucJP
export LC_ALL=ja_JP.eucJP
export LC_CTYPE=ja_JP.eucJP

export GTK_IM_MODULE=ibus

setxkbmap -model jp106 -layout jp

xsetroot -solid "#f0eed8"
xsetroot -cursor_name left_ptr
xrdb ~/.Xresources
xset s off
xset -dpms

xset fp+ /usr/pkg/share/fonts/X11/local
xset fp+ /usr/X11R7/lib/X11/fonts/custom
xset fp rehash

#rxvt -name files -fn -misc-hej-medium-r-semicondensed--13-120-75-75-c-60-iso8859-1 -g 80x16+910+434 -e noice &

ibus-daemon -d &

mixerctl -w outputs.master=80
mixerctl -w inputs.headphones=192

#xbanish &

exec sowm
#exec 2bwm
