#include "musiclist.h"
#include "music.h"

#include <QMimeDatabase>

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MusicList::MusicList()
{

}

void MusicList::addMusicByUrl(const QList<QUrl> &urls)
{
  for(auto musicUrl : urls)
  {
      // 由于添加进来的⽂件不⼀定是歌曲⽂件，因此需要再次筛选出⾳乐⽂件
      QMimeDatabase db;
      QMimeType mime = db.mimeTypeForUrl(musicUrl.toLocalFile());
      if(mime.name() != "audio/mpeg" && mime.name() != "audio/flac")
      {
          //不满足要求的话
          continue;
      }

      //如果是音乐文件
      musicList.push_back(musicUrl);
  }
}

iterator MusicList::begin()
{
    return musicList.begin();
}

iterator MusicList::end()
{
    return musicList.end();
}

iterator MusicList::findMusicById(const QString &musicId)
{
    for(iterator it = begin(); it != end(); ++it)
    {
        if(it->getMusicId() == musicId)
        {
            return it;
        }
    }
    return end();
}

Music *MusicList::findMusicByUrl(const QUrl &url)
{
    for(auto& music : *this)
    {
        if(music.getMusicUrl() == url)
        {
           return &music;
        }
    }
    return nullptr; // Not found
}

void MusicList::writeToDB()
{
    for(auto music : musicList)
    {
        // 让music对象将⾃⼰写⼊数据库
        music.insertMusicToDB();
    }
}

void MusicList::readFromDB()
{
    QString sql("SELECT musicId, musicName, singerName, albumName,\
                        duration, musicUrl, isLike, isHistory \
                 From musicInfo");

    QSqlQuery query;
    if(!query.exec(sql))
    {
        qDebug()<<"数据库查询失败";
        return;
    }

    while (query.next())
    {
        Music music;
        music.setMusicId(query.value(0).toString());
        music.setMusicName(query.value(1).toString());
        music.setSingerName(query.value(2).toString());
        music.setAlbumName(query.value(3).toString());
        music.setDuration(query.value(4).toLongLong());
        music.setMusicUrl(query.value(5).toString());
        music.setIsLike(query.value(6).toBool());
        music.setIsHistory(query.value(7).toBool());

        musicList.push_back(music);
    }
}
