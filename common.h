#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QStringList>

#define SOUNDCOUNT 8

enum SOUND_EVENT
{
    SND_CHANNELMESSAGE=     0,
    SND_PRIVATEMESSAGE=     1,
    SND_JOINED=             2,
    SND_PARTED=             3,
    SND_NOTICE=             4,
    SND_QUIT=               5,
    SND_KICK=               6,
    SND_SENDMSG=            7,
};

struct SETTINGS
{
    /////////////////IRC//////////////////////
    QString nick, username, realname, password;
    QString server;
    unsigned short port;
    QString charset;
    bool autoConnect;
    QStringList autojoinlist;

    ////////////////INTERFACE///////////////////
    bool hideconsole;
    bool trayicon, minimizeonclose;
    QString consolefont;
    QString chatfont;
    bool timestamps;
    QString timestampformat;

    ///////////////SOUNDS/////////////////////////
    bool soundsenabled;
    QString soundfiles[SOUNDCOUNT];
    void setSounFile(unsigned int n, const QString &filename) { soundfiles[n]=filename; }
    void loadList(const QStringList &list);
    QStringList saveList();
    //////////////PATH TO SMILES/////////////////
    QString emoticonspath;
};

extern SETTINGS settings;
extern QString currentnick;
const QString parseNick(const QString &origin);

#endif // COMMON_H
