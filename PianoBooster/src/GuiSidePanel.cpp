
#include <QtGui>

#include "GuiSidePanel.h"
#include "GuiTopBar.h"
#include "TrackList.h"

GuiSidePanel::GuiSidePanel(QWidget *parent, QSettings* settings)
    : QWidget(parent), m_parent(parent)
{
    m_song = 0;
    m_score = 0;
    m_trackList = 0;
    m_topBar = 0;
    m_settings = settings;
    setupUi(this);
}

void GuiSidePanel::init(CSong* songObj, CTrackList* tracks, GuiTopBar* topBar)
{
    m_song = songObj;
    m_trackList = tracks;
    m_topBar = topBar;
    m_trackList->init(songObj, trackList);

    loadBookList();

    followYouRadio->setChecked(true);
    bothHandsRadio->setChecked(true);

    boostSlider->setMinimum(-100);
    boostSlider->setMaximum(100);
}

void GuiSidePanel::loadBookList()
{
    QString currentSong = m_settings->value("CurrentSong").toString();
    QDir dirBooks;
    QString currentBook;
    if (currentSong.isEmpty())
        dirBooks.setPath( QDir::homePath());
    else
    {
        dirBooks.setPath(currentSong);
        dirBooks.cdUp();
        currentBook = dirBooks.dirName();
        dirBooks.cdUp();
    }
    m_bookPath =  dirBooks.path() + '/';

    dirBooks.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList bookNames = dirBooks.entryList();

    bookCombo->clear();

    for (int i = 0; i < bookNames.size(); ++i)
    {
        bookCombo->addItem( bookNames.at(i));
        if (bookNames.at(i) == currentBook)
            bookCombo->setCurrentIndex(i);
    }
    on_bookCombo_activated(-1);
}

void GuiSidePanel::openSongFile(QString filename)
{
    QDir dirBooks(filename);

    if (!QFile::exists(filename))
        return;

    m_settings->setValue("CurrentSong", filename);

    loadBookList();

    loadSong(filename);
}

void GuiSidePanel::loadSong(QString filename)
{
    m_song->loadSong(filename);
    if (m_trackList)
    {
        m_trackList->refresh();
        m_topBar->refresh(true);
    }
    m_parent->setWindowTitle("Piano Booster - " + m_song->getSongTitle());
}

void GuiSidePanel::on_bookCombo_activated (int index)
{
    QDir dirSongs = QDir(m_bookPath + bookCombo->currentText());
    dirSongs.setFilter(QDir::Files);
    QString currentSong;

    if (index < 0)
        currentSong = m_settings->value("CurrentSong").toString();

    songCombo->clear();
    QStringList songNames = dirSongs.entryList();

    int j = 0;
    for (int i = 0; i < songNames.size(); ++i)
    {
        if ( songNames.at(i).endsWith(".mid", Qt::CaseInsensitive ) ||
             songNames.at(i).endsWith(".kar", Qt::CaseInsensitive ) )
        {
            songCombo->addItem( songNames.at(i));
            if (m_bookPath + bookCombo->currentText() + '/' + songNames.at(i) == currentSong)
                songCombo->setCurrentIndex(j);
            j++;
        }
    }
    on_songCombo_activated(index); // Now load the selected song
}

void GuiSidePanel::on_songCombo_activated(int index) {
    if (!m_song) return;

    QString str = m_bookPath + bookCombo->currentText() + '/' + songCombo->currentText();

    loadSong(str);
}

void GuiSidePanel::on_rightHandRadio_toggled (bool checked)
{
    if (!m_song ) return;

    if (checked)
        m_song->setActiveHand(PB_PART_right);
}

void GuiSidePanel::on_bothHandsRadio_toggled (bool checked)
{
    if (!m_song ) return;

    if (checked)
        m_song->setActiveHand(PB_PART_both);
}

void GuiSidePanel::on_leftHandRadio_toggled (bool checked)
{
    if (!m_song ) return;

    if (checked)
        m_song->setActiveHand(PB_PART_left);
}

