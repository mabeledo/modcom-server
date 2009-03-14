#!/bin/sh
 
NONULL="-nullassign"
BOOLINT="+boolint"
CHARINT="+charintliteral +charint"
MUSTDEFINE="-mustdefine"
COMPDEF="-compdef"
UNRECOG="-unrecog"
NULLPASS="-nullpass"
 
ARGS="${NONULL} ${BOOLINT} ${CHARINT} ${COMPDEF} ${UNRECOG} ${NULLPASS}"
 
splint -I `pkg-config --cflags glib-2.0` -I `pkg-config --cflags gmodule-2.0` -I `pkg-config --cflags gthread-2.0` $ARGS $1
