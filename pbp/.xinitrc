export LANG=ja_JP.UTF-8
export LC_ALL=ja_JP.UTF-8
export LC_CTYPE=ja_JP.UTF-8

xsetroot -solid "#0e0c05"
#hsetroot -cover ~/img/leaf.jpeg
xsetroot -cursor_name left_ptr

xset fp+ /usr/X11R7/lib/X11/fonts/custom
xset fp rehash

export GTK_IM_MODULE=uim
export XMODIFIERS=@im=uim

xrdb $HOME/.config/.Xresources

xbanish &

filecmd

#exec /usr/pkg/bin/ctwm -W
exec 2bwm
