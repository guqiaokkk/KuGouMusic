#include "music.h"
#include <QUuid>
#include <QMediaPlayer>
#include <QCoreApplication>


Music::Music(const QUrl &url)
     : isLike(false)
     , isHistory(false)
     , musicUrl(url)
{
    musicId = QUuid::createUuid().toString();
    parseMediaMetaData();
}

void Music::setIsLike(bool isLike)
{
    this->isLike = isLike;
}

void Music::setIsHistory(bool isHistory)
{
    this->isHistory = isHistory;
}

void Music::setMusicName(const QString &musicName)
{
    this->musicName = musicName;
}

void Music::setSingerName(const QString &singerName)
{
    this->singerName = singerName;
}

void Music::setAlbumName(const QString &albumName)
{
    this->albumName = albumName;
}

void Music::setDuration(const qint64 duration)
{
    this->duration = duration;
}

void Music::setMusicUrl(const QUrl &url)
{
    this->musicUrl = url;
}

void Music::setMusicId(const QString &musicId)
{
    this->musicId = musicId;
}

bool Music::getIsLike()
{
    return isLike;
}

bool Music::getIsHistory()
{
    return isHistory;
}

QString Music::getMusicName()
{
    return musicName;
}

QString Music::getSingerName()
{
    return singerName;
}

QString Music::getAlbumName()
{
    return albumName;
}

qint64 Music::getDuration()
{
    return duration;
}

QUrl Music::getMusicUrl()
{
    return musicUrl;
}

QString Music::getMusicId()
{
    return musicId;
}

void Music::parseMediaMetaData()
{
    // 解析时候需要读取歌曲数据，读取歌曲⽂件需要⽤到QMediaPlayer类
    QMediaPlayer player;
    player.setMedia(musicUrl);

    // 媒体元数据解析需要时间，只有等待解析完成之后，才能提取⾳乐信息，此处循环等待
    // 循环等待时：主界⾯消息循环就⽆法处理了，因此需要在等待解析期间，让消息循环继续处理
    while (!player.isMetaDataAvailable())
    {
        QCoreApplication::processEvents();
    }

    // 解析媒体元数据结束，提取元数据信息
    if(player.isMetaDataAvailable())
    {
        musicName = player.metaData("Title").toString();
        singerName = player.metaData("Author").toString();
        albumName = player.metaData("AlbumTitle").toString();
        duration = player.duration();

        QString filename = musicUrl.fileName();
         // 找-的位置
        int index = filename.indexOf('-');

        if(musicName.isEmpty())
        {
            if(index != -1)
            {
                musicName = filename.mid(0, index).trimmed();
            }
            else
            {
                musicName = filename.mid(0, filename.indexOf('.')).trimmed();
            }
        }

        if(singerName.isEmpty())
        {
            if(index != -1)
            {
                singerName = filename.mid(index + 1, filename.indexOf('.')-index-1).trimmed();
            }
            else
            {
                singerName = "未知歌手";
            }
        }

        if(albumName.isEmpty())
        {
            if(index != -1)
            {
                albumName = "未知专辑";
            }
        }
        qDebug()<<musicName<<" "<<singerName<<" "<<albumName<<" "<<duration;
    }
}

