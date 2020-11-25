export GTK_IM_MODULE=uim
export QT_IM_MODULE=uim
export XMODIFIERS=@im=uim

hsetroot -solid "#f0f0cc"
xrdb ~/.Xresources
#fcitx &
uim-xim &
picom &

rxvt -g 80x16+902+426 -e noice &

exec dwm 
