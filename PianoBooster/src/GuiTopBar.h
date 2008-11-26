#ifndef __GUITOPBAR_H__
#define __GUITOPBAR_H__

#include <QtGui>

#include "Song.h"
#include "Score.h"
#include "TrackList.h"

#include "ui_GuiTopBar.h"

class GuiTopBar : public QWidget, private Ui::GuiTopBar
{
    Q_OBJECT

public:
    GuiTopBar(QWidget *parent = 0);

    void init(CSong* songObj, CTrackList* trackList);

    void refresh(bool reset);

    void setPlayButtonState(bool checked, bool atTheEnd = false);

private slots:
    void on_playButton_clicked(bool clicked);
    void on_playFromStartButton_clicked(bool clicked);
    void on_speedSpin_valueChanged(int speed);

    void on_transposeSpin_valueChanged(int value);
    void on_keyCombo_activated(int index);

    void on_majorCombo_activated(int index)
    {
        reloadKeyCombo((index == 0)?true:false);
    }

private:
    void reloadKeyCombo(bool major);

    CSong* m_song;

    bool m_atTheEndOfTheSong;
};

#endif //__GUITOPBAR_H__
