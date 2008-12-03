#ifndef __GUISIDEPANEL_H__
#define __GUISIDEPANEL_H__

#include <QtGui>

#include "Song.h"
#include "Score.h"
#include "TrackList.h"

#include "ui_GuiSidePanel.h"

class GuiTopBar;

class GuiSidePanel : public QWidget, private Ui::GuiSidePanel
{
    Q_OBJECT

public:
    GuiSidePanel(QWidget *parent, QSettings* settings);

    void init(CSong* songObj, CTrackList* trackList, GuiTopBar* topBar);
    void openSongFile(QString filename);
    QString getCurrentSongFileName()
    {
        return m_bookPath + bookCombo->currentText() + '/' + songCombo->currentText();
    }

private slots:
    void on_songCombo_activated (int index);
    void on_bookCombo_activated (int index);
    void on_rightHandRadio_toggled (bool checked);
    void on_bothHandsRadio_toggled (bool checked);
    void on_leftHandRadio_toggled (bool checked);

    void on_trackList_currentRowChanged(int currentRow) {
        if (m_trackList) m_trackList->currentRowChanged(currentRow);
    }

    void on_boostSlider_valueChanged(int value) {
        if (m_song) m_song->boostVolume(value);
    }

    void on_listenRadio_toggled (bool checked)
    {
        if (!m_song || !checked) return;
        m_song->setPlayMode(PB_PLAY_MODE_listen);
    }

    void on_followYouRadio_toggled (bool checked)
    {
        if (!m_song || !checked) return;
        m_song->setPlayMode(PB_PLAY_MODE_followYou);
    }

    void on_playAlongRadio_toggled (bool checked)
    {
        if (!m_song || !checked) return;
        m_song->setPlayMode(PB_PLAY_MODE_playAlong);
    }

private:
    void loadBookList();
    void loadSong(QString filename);

    CSong* m_song;
    CScore* m_score;
    QString m_bookPath;
    CTrackList* m_trackList;
    GuiTopBar* m_topBar;
    QSettings* m_settings;
    QWidget *m_parent;
};

#endif //__GUISIDEPANEL_H__
