#!/bin/sh

# This script updates ts files of pianobooster, but does not touch ts files of music
# License: same with Pianobooster
# Author: Alexey Loginov <alexl@mageia.org>

if [ ! -z "`lupdate -version 2>/dev/null`" ]
then
  lupdate="lupdate"
fi

if [ ! -z "`lupdate-qt5 -version 2>/dev/null`" ]
then
  lupdate="lupdate-qt5"
fi

if [ -z "$lupdate" ]
then
  echo "lupdate was not found!"
  exit 1
fi

pushd ../
  $lupdate pianobooster.pro -no-obsolete -ts translations/pianobooster_blank.ts
  #$lupdate pianobooster.pro -no-obsolete -ts translations/pianobooster_*.ts
popd
