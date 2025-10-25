#ifndef CPRLIB_H
#define CPRLIB_H

#include <QList>
#include <QSoundEffect>
#include <QString>

QString getContent(const QString& suffix);

extern QList<QSoundEffect*> soundList;
void addSound(const QString& path, bool hasPrefix = 0);
void sweepSound();

#endif // CPRLIB_H
