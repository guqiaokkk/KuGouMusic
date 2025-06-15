#include "music.h"
#include <QUuid>
#include <QMediaPlayer>
#include <QCoreApplication>

#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <algorithm> // for std::remove_if



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

QString Music::getLrcFilePath() const
{
    // ⾳频⽂件和LRC⽂件在⼀个⽂件夹下. （这里也可以歌曲放在一个文件夹，歌词放在一个文件夹）
    // 直接将⾳频⽂件的后缀替换为.lrc
    QFileInfo musicFileInfo(musicUrl.toLocalFile());
        QDir musicDir = musicFileInfo.dir();
        QString musicBaseName = musicFileInfo.baseName(); // e.g., "血纯茗雅 - 光与信仰"

        // --- 查找策略 1: 基于关键词的模糊匹配 (最强大) ---
        // 1.1 提取关键词
        // 使用正则表达式按空格或-分割，并去除空字符串
        QRegularExpression separator("[\\s-]+");
        QStringList keywords = musicBaseName.split(separator, Qt::SkipEmptyParts);

        // 1.2 移除太短的关键词，避免误判 (比如 "a", "of" 等)
        keywords.erase(std::remove_if(keywords.begin(), keywords.end(),
                                      [](const QString& s) { return s.length() < 2; }),
                       keywords.end());

        if (!keywords.isEmpty())
        {
            // 1.3 获取目录下所有的.lrc文件
            QStringList allLrcFiles = musicDir.entryList(QStringList() << "*.lrc", QDir::Files);

            // 1.4 遍历所有lrc文件，寻找包含所有关键词的那个
            for (const QString& lrcFileName : allLrcFiles)
            {
                bool allKeywordsMatch = true;
                for (const QString& keyword : keywords)
                {
                    // 使用不区分大小写的包含检查，更健壮
                    if (!lrcFileName.contains(keyword, Qt::CaseInsensitive))
                    {
                        allKeywordsMatch = false;
                        break; // 只要有一个关键词不匹配，就检查下一个lrc文件
                    }
                }

                if (allKeywordsMatch)
                {
                    // 找到了！这个lrc文件包含了所有关键词
                    qDebug() << "关键词匹配成功:" << lrcFileName;
                    return musicDir.filePath(lrcFileName);
                }
            }
        }


        // --- 查找策略 2: 前缀匹配 (上一版的方案，作为备用) ---
        QStringList filters;
        filters << musicBaseName + "*.lrc";
        QStringList lrcFiles = musicDir.entryList(filters, QDir::Files);
        if (!lrcFiles.isEmpty())
        {
            qDebug() << "前缀匹配成功:" << lrcFiles.first();
            return musicDir.filePath(lrcFiles.first());
        }

        // --- 查找策略 3: 精确匹配 (最原始的方案，作为最后防线) ---
        QString exactMatchPath = musicDir.filePath(musicBaseName + ".lrc");
        if (QFile::exists(exactMatchPath)) {
            qDebug() << "精确匹配成功:" << exactMatchPath;
            return exactMatchPath;
        }

        // 如果所有方法都失败了，就返回空字符串
        qDebug() << "所有歌词匹配策略均失败，未找到歌词文件。";
        return QString();
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

