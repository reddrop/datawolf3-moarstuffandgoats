#include "common.h"
#
SETTINGS settings;
QString currentnick;

const QString parseNick(const QString &origin)
{
    int pos = origin.indexOf(QChar('!'), 0);
    if (pos<0)
        return origin;
    return origin.left(pos);
}


//////////////////////sounds////////////////////////
void SETTINGS::loadList(const QStringList &list)
{
    int size = qMin(SOUNDCOUNT, list.size());
    for (int i=0; i<size; i++)
        soundfiles[i] = list.at(i);
}

QStringList SETTINGS::saveList()
{
    QStringList result;
    for (int i=0; i<SOUNDCOUNT; i++)
        result << soundfiles[i];
    return result;
}
