setxkbmap -option ctrl:nocaps
setxkbmap -option keypad:pointerkeys
setxkbmap -model jp106 -layout jp

export XMODIFIERS=@im=xim
export GTK_IM_MODULE=xim

xrdb ~/.Xresources
xsetroot -solid "antiquewhite"

xset -dpms
xset s off
unclutter --timeout 1 &

exec dwm
