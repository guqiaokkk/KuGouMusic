#include "music.h"
#include <QUuid>
#include <QMediaPlayer>
#include <QCoreApplication>

#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <algorithm> // for std::remove_if

#include <QSqlQuery>
#include <QSqlError>

Music::Music()
    : isLike(false)
    , isHistory(false)
{

}

Music::Music(const QUrl &url)
     : isLike(false)
     , isHistory(false)
     , musicUrl(url)
{
    // 读取url对应的歌曲文件的信息，解析出元数据
    // 歌曲名称、歌曲作者、歌曲专辑、歌曲持续时长
    musicId = QUuid::createUuid().toString();
    parseMediaMetaData();
    findAndSetLrcPath();
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



// 将当前Music对象更新到数据库
void Music::insertMusicToDB()
{
    // 1. 检测music是否在数据库中存在
    QSqlQuery query;

    query.prepare("SELECT EXISTS(SELECT 1 FROM musicInfo WHERE musicId = ?)");
    query.addBindValue(musicId);
    if(!query.exec())
    {
        qDebug() << "查询失败" << query.lastError().text();
        return;
    }

    if(query.next())//此时query里保存的 1 或 0
    {
        bool isExists = query.value(0).toBool();
        if(isExists)
        {
            // musicId的歌曲已经存在
            // 2. 存在：不需要再插⼊musci对象，此时只需要将isLike和isHistory属性进⾏更新
            query.prepare("UPDATE musicInfo SET isLike = ?, isHistory = ?, lrcPath = ? WHERE musicId = ?");
            query.addBindValue(isLike ? 1 : 0);
            query.addBindValue(isHistory ? 1 : 0);
            query.addBindValue(lrcPath);
            query.addBindValue(musicId);
            if(!query.exec())
            {
                qDebug()<<"更新失败: "<<query.lastError().text();
            }
            qDebug()<<"更新music信息: "<<musicName<<" "<<musicId;
        }
        else
        {
            // 3. 不存在：直接将music的属性信息插⼊数据库
            query.prepare("INSERT INTO musicInfo(musicId, musicName, singerName, albumName,\
                                                 musicUrl, duration, lrcPath, isLike, isHistory)\
                                          VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
            query.addBindValue(musicId);
            query.addBindValue(musicName);
            query.addBindValue(singerName);
            query.addBindValue(albumName);
            query.addBindValue(musicUrl.toLocalFile());
            query.addBindValue(duration);
            query.addBindValue(lrcPath);
            query.addBindValue(isLike ? 1 : 0);
            query.addBindValue(isHistory ? 1 : 0);

            if(!query.exec())
            {
                qDebug()<<"插⼊失败: "<<query.lastError().text();
                return;
            }
            qDebug()<<"插⼊music信息: "<<musicName<<" "<<musicId;
        }
    }
}

void Music::setLrcPath(const QString &path)
{
    this->lrcPath = path;
}

QString Music::getLrcPath() const
{
     return this->lrcPath;
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

void Music::findAndSetLrcPath()
{
    // 检查URL是否为本地文件，如果不是，直接设置为空并返回
    if (!musicUrl.isLocalFile()) {
        qDebug() << "[LRC Finder] 错误: URL不是一个本地文件路径，无法查找歌词。" << musicUrl;
        setLrcPath("");
        return;
    }

    QFileInfo musicFileInfo(musicUrl.toLocalFile());
    QDir musicDir = musicFileInfo.dir();
    QString musicBaseName = musicFileInfo.baseName(); // e.g., "血纯茗雅 - 光与信仰"

    qDebug() << "[LRC Finder] 开始为歌曲寻找歌词: " << musicBaseName;
    qDebug() << "[LRC Finder] 目录: " << musicDir.path();

    // --- 查找策略 1: 基于关键词的模糊匹配 (最强大) ---
    // 1.1 提取关键词
    QRegularExpression separator("[\\s-]+");
    QStringList keywords = musicBaseName.split(separator, Qt::SkipEmptyParts);

    // 1.2 移除太短的关键词
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
                if (!lrcFileName.contains(keyword, Qt::CaseInsensitive))
                {
                    allKeywordsMatch = false;
                    break;
                }
            }

            if (allKeywordsMatch)
            {
                qDebug() << "[LRC Finder] 策略1 (关键词匹配) 成功:" << lrcFileName;
                // 而是 setLrcPath 并退出函数
                setLrcPath(musicDir.filePath(lrcFileName));
                return; // 找到了，任务完成，退出函数
            }
        }
    }


    // --- 查找策略 2: 前缀匹配 (备用) ---
    QStringList filters;
    filters << musicBaseName + "*.lrc";
    QStringList lrcFiles = musicDir.entryList(filters, QDir::Files);
    if (!lrcFiles.isEmpty())
    {
        qDebug() << "[LRC Finder] 策略2 (前缀匹配) 成功:" << lrcFiles.first();
        // setLrcPath 并退出函数
        setLrcPath(musicDir.filePath(lrcFiles.first()));
        return; // 找到了，任务完成，退出函数
    }

    // --- 查找策略 3: 精确匹配 (最后防线) ---
    QString exactMatchPath = musicDir.filePath(musicBaseName + ".lrc");
    if (QFile::exists(exactMatchPath)) {
        qDebug() << "[LRC Finder] 策略3 (精确匹配) 成功:" << exactMatchPath;
        //  setLrcPath 并退出函数
        setLrcPath(exactMatchPath);
        return; // 找到了，任务完成，退出函数
    }

    // 如果所有方法都失败了
    qDebug() << "[LRC Finder] 所有策略均失败，未找到歌词文件。";
    // 设置为空字符串
    setLrcPath("");
}
