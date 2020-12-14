#!/bin/sh
set -e

po4a-gettextize -f text \
-m BeginnerCourse/01-StartWithMiddleC.md \
-m BeginnerCourse/02-ChordOfCMajor.md \
-m BeginnerCourse/03-UpAndDown.md \
-m BeginnerCourse/04-ClairDeLaLune.md \
-m BeginnerCourse/05-ChordOfFMajor.md \
-m BeginnerCourse/06-DownAndUp.md \
-m BeginnerCourse/07-IntervalsRightHand.md \
-m BeginnerCourse/08-IntervalsLeftHand.md \
-m BoosterMusic/01-ClairDeLaLune.md \
-m BoosterMusic/02-LavendersBlue.md \
-m BoosterMusic/03-SkipToMyLou.md \
-m BoosterMusic/04-FrereJacques.md \
-m BoosterMusic/05-ScarboroughFair.md \
-m BoosterMusic/06-Greensleeves.md \
-m BoosterMusic/07-AmazingGrace.md \
-p translations/BoosterMusic_blank.pot


msgmerge --update --no-fuzzy-matching translations/BoosterMusic_cs.po translations/BoosterMusic_blank.pot
msgmerge --update --no-fuzzy-matching translations/BoosterMusic_de.po translations/BoosterMusic_blank.pot
msgmerge --update --no-fuzzy-matching translations/BoosterMusic_es.po translations/BoosterMusic_blank.pot
msgmerge --update --no-fuzzy-matching translations/BoosterMusic_fr.po translations/BoosterMusic_blank.pot
msgmerge --update --no-fuzzy-matching translations/BoosterMusic_nb.po translations/BoosterMusic_blank.pot
msgmerge --update --no-fuzzy-matching translations/BoosterMusic_pl.po translations/BoosterMusic_blank.pot
msgmerge --update --no-fuzzy-matching translations/BoosterMusic_ru.po translations/BoosterMusic_blank.pot
msgmerge --update --no-fuzzy-matching translations/BoosterMusic_uk.po translations/BoosterMusic_blank.pot
msgmerge --update --no-fuzzy-matching translations/BoosterMusic_zh.po translations/BoosterMusic_blank.pot
