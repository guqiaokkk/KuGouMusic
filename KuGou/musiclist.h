#ifndef MUSICLIST_H
#define MUSICLIST_H

#include "music.h"

#include <QVector>
#include <QUrl>

#include <QSet>


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

    void writeToDB();

    void readFromDB();

private:
    QVector<Music> musicList;

    //歌曲的路径保存(同⼀个电脑上，⽂件路径不可能重复)，在进⾏歌曲加载时，先检测歌曲⽂件是否存在，如果不存在则添加否则不添加
    QSet<QString> musicPaths;

};

#endif // MUSICLIST_H
