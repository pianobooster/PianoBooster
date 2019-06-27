#!/bin/sh

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
popd
