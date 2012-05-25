#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QStringList>
#include "common.h"

void playSound(SOUND_EVENT id);
void playSound(const QString &filename);


#endif // SOUNDPLAYER_H
