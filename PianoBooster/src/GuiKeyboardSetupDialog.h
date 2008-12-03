#ifndef __GUILEYBOARDSETUPDIALOG_H__
#define __GUILEYBOARDSETUPDIALOG_H__

#include <QtGui>

#include "Song.h"


#include "ui_GuiKeyboardSetupDialog.h"

class GuiKeyboardSetupDialog : public QDialog, private Ui::GuiKeyboardSetupDialog
{
    Q_OBJECT

public:
    GuiKeyboardSetupDialog(QWidget *parent = 0);

    void init(CSong* song, QSettings* settings);

private slots:
    void accept();
    void reject();


    void on_rightTestButton_pressed() {
        m_song->testWrongNoteSound(false);
        m_song->pcKeyPress( 'x', true);
    }
    void on_rightTestButton_released() {
        m_song->pcKeyPress( 'x', false);
    }
    void on_wrongTestButton_pressed() {
        m_song->testWrongNoteSound(true);
        m_song->pcKeyPress( 'x', true);
    }
    void on_wrongTestButton_released() {
        m_song->pcKeyPress( 'x', false);
    }

    void on_resetButton_clicked(bool clicked) {
        lowestNoteEdit->setText("0");
        highestNoteEdit->setText("127");
        updateInfoText();
    }

    void on_rightSoundCombo_activated (int index){ updateSounds(); }
    void on_wrongSoundCombo_activated (int index){ updateSounds(); }
    void on_lowestNoteEdit_editingFinished(){updateInfoText();}
    void on_highestNoteEdit_editingFinished(){updateInfoText();}
private:
    void keyPressEvent ( QKeyEvent * event );
    void keyReleaseEvent ( QKeyEvent * event );

    void updateSounds (){
        m_song->setPianoSoundPatches(rightSoundCombo->currentIndex(),
                                     wrongSoundCombo->currentIndex());
    }

    void updateInfoText();

    QSettings* m_settings;
    CSong* m_song;
};

#endif //__GUILEYBOARDSETUPDIALOG_H__
