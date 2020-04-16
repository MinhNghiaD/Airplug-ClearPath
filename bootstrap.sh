#!/bin/bash

set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR


SOURCEDIR=$PWD

command -v qtpaths >/dev/null 2>&1 || { echo >&2 "This script require qtpaths CLI tool from Qt5 project but it's not installed. Aborting."; exit 1; }

MAKEFILES_TYPE='Unix Makefiles'
BUILDDIR=$PWD"/build"
MESSAGE="Now run make in $BUILDDIR. "

if [[ "$ARCH" = "x86_64" ]] ; then
    LIBPATH="lib64"
elif [[ "$ARCH" = "i686" ]] ; then
    LIBPATH="lib"
fi

QT_INSTALL_PREFIX=`qtpaths --install-prefix`
QT_PLUGIN_INSTALL_DIR=`qtpaths --plugin-dir`
export PATH=$QT_INSTALL_PREFIX/bin:$PATH


echo "Qt5     Install Path : $QT_INSTALL_PREFIX"
echo "Build Directory      : $BUILDDIR"

rm -rf $BUILDDIR
mkdir -p $BUILDDIR
cd $BUILDDIR

cmake ..
