#include "soundstable.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QIcon>

SoundsTable::SoundsTable(QWidget *parent) :
    QTableWidget(parent)
{
    setColumnCount(3);
    QStringList header;
    header << tr("Sound file") << tr("Select") << tr("Play");
    setHorizontalHeaderLabels(header);
    horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
    horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);

    verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    setSortingEnabled(false);

    setRowCount(SOUNDCOUNT);
    header.clear();
    header << tr("Channel Message")
            << tr("Private message")
            << tr("Join channel")
            << tr("Leave channel")
            << tr("Notice")
            << tr("Quit IRC")
            << tr("Kick")
            << tr("Outgoing message");
    setVerticalHeaderLabels(header);

    QIcon foldericon(":/icons/folder.png");
    QIcon playicon(":/icons/play.png");

    for (int i=0; i<SOUNDCOUNT; i++)
    {
        browsebuttons[i] = new MyPushButton(foldericon);
        browsebuttons[i]->setFlat(true);
        browsebuttons[i]->number = i;
        connect(browsebuttons[i], SIGNAL(clicked()), this, SLOT(selectSoundFile()));
        setCellWidget(i, 1, browsebuttons[i]);

        playbuttons[i] = new MyPushButton(playicon);
        playbuttons[i]->setFlat(true);
        playbuttons[i]->number = i;
        connect(playbuttons[i], SIGNAL(clicked()), this, SLOT(playPressed()));
        setCellWidget(i, 2, playbuttons[i]);
    }

    setSelectionMode(QAbstractItemView::NoSelection);
}

SoundsTable::~SoundsTable()
{
    for (int i=0; i<SOUNDCOUNT; i++)
    {
        delete browsebuttons[i];
        delete playbuttons[i];
    }
}

void SoundsTable::readSounds()
{
    for (int i=0; i<SOUNDCOUNT; i++)
    {
        QTableWidgetItem *itm = item(i, 0);
        if (itm)
            itm->setText(settings.soundfiles[i]);
        else
            setItem(i, 0, new QTableWidgetItem(settings.soundfiles[i]));
    }
}

void SoundsTable::writeSounds()
{
    for (int i=0; i<SOUNDCOUNT; i++)
    {
        QTableWidgetItem *itm = item(i, 0);
        if (itm)
            settings.soundfiles[i] = itm->text();
        else
            settings.soundfiles[i] = QString();
    }
}

void SoundsTable::selectSoundFile()
{
    MyPushButton *button = qobject_cast<MyPushButton *> (sender());
    if (!button)
        return;
    QString filename = QFileDialog::getOpenFileName(this, tr("Select sound file"), currentdir,
                                                    tr("Sound files (*.wav);;All files (*.*)"),
                                                    &currentfilter);
    if (filename.isEmpty())
        return;

    QDir dir(filename);
    dir.cdUp();
    currentdir = dir.absolutePath();

    QTableWidgetItem *itm = item(button->number, 0);
    if (itm)
        itm->setText(filename);
    else
        setItem(button->number, 0, new QTableWidgetItem(filename));
}

void SoundsTable::playPressed()
{
    MyPushButton *button = qobject_cast<MyPushButton *> (sender());
    if (!button)
        return;
    QTableWidgetItem *itm = item(button->number, 0);
    if (itm)
    {
        playSound(itm->text());
    }
}
