#!/usr/bin/env bash

grSimArgs="$@"

if [[ "$1" == "vnc" ]]; then
  echo "Launch in VNC mode"
  grSimArgs="${@:1}"
  if [[ -z "${VNC_PASSWORD}" ]]; then
    VNC_PASSWORD=vncpassword
  fi
  if [[ -z "${VNC_GEOMETRY}" ]]; then
    VNC_GEOMETRY=1280x1024
  fi

  mkdir ~/.vnc
  x11vnc -storepasswd "${VNC_PASSWORD}" ~/.vnc/passwd

  cat <<EOF >>~/.xinitrc
#!/bin/sh
exec /usr/local/bin/grSim --qwindowgeometry ${VNC_GEOMETRY} $grSimArgs
EOF
  chmod 700 ~/.xinitrc

  export X11VNC_CREATE_GEOM="${VNC_GEOMETRY}"
  exec x11vnc -forever -usepw -display WAIT:cmd=FINDCREATEDISPLAY-Xvfb
else
  echo "Launch in headless mode"
  exec /usr/local/bin/grSim --headless -platform offscreen $grSimArgs
fi
