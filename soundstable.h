#ifndef SOUNDSTABLE_H
#define SOUNDSTABLE_H

#include <QTableWidget>
#include <QPushButton>
#include "soundplayer.h"

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    MyPushButton(QWidget * parent = 0) : QPushButton(parent) {}
    MyPushButton(const QString & text, QWidget * parent = 0) : QPushButton(text, parent) {}
    MyPushButton(const QIcon & icon, const QString & text = QString(), QWidget * parent = 0) :
            QPushButton(icon, text, parent) {}

    int number;
};

class SoundsTable : public QTableWidget
{
    Q_OBJECT
    MyPushButton *playbuttons[SOUNDCOUNT];
    MyPushButton *browsebuttons[SOUNDCOUNT];
    QString currentdir;
    QString currentfilter;

public:
    explicit SoundsTable(QWidget *parent = 0);
    ~SoundsTable();
    void readSounds();
    void writeSounds();

private slots:
    void selectSoundFile();
    void playPressed();
};

#endif // SOUNDSTABLE_H
