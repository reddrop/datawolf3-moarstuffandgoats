#include "soundplayer.h"
#include <QProcess>
#include <QSound>

void playSound(SOUND_EVENT id)
{
    if (!settings.soundsenabled)
        return;
    if (id>=SOUNDCOUNT)
        return;
    if (settings.soundfiles[id].isEmpty())
        return;
    if (QSound::isAvailable())
        QSound::play(settings.soundfiles[id]);
    else
    {
        QStringList params;
        params << "--quiet" << settings.soundfiles[id];
        QProcess::startDetached("aplay", params);
    }
}

void playSound(const QString &filename)
{
    if (filename.isEmpty())
        return;
    if (QSound::isAvailable())
        QSound::play(filename);
    else
    {
        QStringList params;
        params << "--quiet" << filename;
        QProcess::startDetached("aplay", params);
    }
}
