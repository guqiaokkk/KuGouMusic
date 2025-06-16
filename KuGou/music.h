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

    QString getLrcFilePath()const;

    void insertMusicToDB();

    //歌词相关
    void setLrcPath(const QString &path);
    QString getLrcPath() const;

private:
    void parseMediaMetaData();

     void findAndSetLrcPath(); //lrc搜寻， 在构造时调用，查找并设置歌词路径

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

     QString lrcPath; //歌词的文件地址
};

#endif // MUSIC_H
