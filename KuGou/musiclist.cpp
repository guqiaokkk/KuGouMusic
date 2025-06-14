#include "musiclist.h"

#include <QMimeDatabase>

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
