#ifndef MUSICLIST_H
#define MUSICLIST_H

#include "music.h"

#include <QVector>
#include <QUrl>

typedef typename QVector<Music>::iterator iterator;

class MusicList
{
public:
    MusicList();

    void addMusicByUrl(const QList<QUrl> &urls);

    iterator begin();
    iterator end();
    iterator findMusicById(const QString &musicId);
    Music *findMusicByUrl(const QUrl& url);


private:
    QVector<Music> musicList;
};

#endif // MUSICLIST_H
