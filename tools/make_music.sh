#/bin/sh

pushd ../

version=`cat src/Settings.cpp|grep 'MUSIC_RELEASE ='|cut -d "=" --fields=2|sed "s|.* ||g"|cut -d ";" --fields=1`

rm -rf music/BoosterMusicBooks$version

mkdir -p music/BoosterMusicBooks$version/BeginnerCourse/InfoPages
mkdir -p music/BoosterMusicBooks$version/BoosterMusic/InfoPages

cp -f doc/courses/BeginnerCourse/*.mid music/BoosterMusicBooks$version/BeginnerCourse/
cp -f doc/courses/BoosterMusic/*.mid music/BoosterMusicBooks$version/BoosterMusic/

for file in `find ./music-src -name ??-*.md`
do
  file_html=`echo $file|sed "s|_en.md|_en.html|g"|sed "s|./music-src|music/BoosterMusicBooks$version|g"|sed "s|/0|/InfoPages/0|g"`
  markdown $file > $file_html
  awk 'NR>1{printf "\n"} {printf $0}' $file_html > "$file_html"_tmp
  mv -f "$file_html"_tmp $file_html
  sed -i 's|<p><strong>Hint:|<font color="#ff0000"><p><b>Hint:|g' $file_html
  printf "</font>" >> $file_html
  printf "</body>" >> $file_html
  sed -i '1s/^/<body bgcolor=#FFFFC0>/' $file_html
done

pushd music
rm -f BoosterMusicBooks.zip
zip -r BoosterMusicBooks.zip BoosterMusicBooks$version
popd

popd

./conv_html_to_ts.pl ../music/BoosterMusicBooks$version
mv music_blank.ts ../translations/
rm -rf ../music/BoosterMusicBooks$version
