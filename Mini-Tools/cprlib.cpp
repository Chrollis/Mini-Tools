#include "cprlib.h"

QString getContent(const QString& suffix)
{
#ifdef QT_DEBUG
    return "C:/Users/Chrollis/Desktop/Content/" + suffix;
#else
    return "./Content/" + suffix;
#endif
}

QList<QSoundEffect*> soundList;
void addSound(const QString& path, bool hasPrefix)
{
    QSoundEffect* sound = new QSoundEffect;
    if (hasPrefix) {
        sound->setSource(QUrl::fromLocalFile(path));
    } else {
        sound->setSource(QUrl::fromLocalFile(getContent(path)));
    }
    sound->setLoopCount(1);
    sound->setVolume(1.0);
    sound->play();
    soundList.prepend(sound);
}

void sweepSound()
{
    for (auto it = soundList.begin(); it != soundList.end();) {
        if (!(*it)->isPlaying()) {
            delete *it;
            it = soundList.erase(it);
        } else {
            it++;
        }
    }
}
