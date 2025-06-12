#ifndef MUSIC_H
#define MUSIC_H

#include <QUrl>
#include <QString>

class Music
{
public:
    Music();
    Music(const QUrl &url);

    void setIsLike(bool isLike);
    void setIsHistory(bool isHistory);
    void setMusicName(const QString &musicName);
    void setSingerName(const QString &singerName);
    void setAlbumName(const QString &albumName);
    void setDuration(const qint64 duration);
    void setMusicUrl(const QUrl &url);
    void setMusicId(const QString &musicId);

    bool getIsHistory();
    bool getIsLike();
    QString getMusicName();
    QString getSingerName();
    QString getAlbumName();
    qint64 getDuration();
    QUrl getMusicUrl();
    QString getMusicId();

private:
    void parseMediaMetaData();

private:
    bool isLike;
    bool isHistory;

    //音乐基本信息
    QString musicName;
    QString singerName;
    QString albumName;
    qint64 duration;

    //标识歌曲的唯一性
    QString musicId;

    //音乐在磁盘的位置
    QUrl musicUrl;
};

#endif // MUSIC_H
