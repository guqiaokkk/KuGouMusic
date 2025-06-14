#ifndef RECENTPAGE_H
#define RECENTPAGE_H



#include "musiclist.h"

#include <QWidget>
#include <QMediaPlaylist>


namespace Ui {
class RecentPage;
}

class RecentPage : public QWidget
{
    Q_OBJECT

public:
    explicit RecentPage(QWidget *parent = nullptr);
    ~RecentPage();
    QString typeName();

    void addMusicToMusicPage(MusicList &musicList);
    void reFresh(MusicList &musicList);

    void addMusicToPlayer(MusicList &musicList, QMediaPlaylist *playList);


signals:
    void updatalikeMusic(bool isLike, QString musicId);

    void playAll(QString type); //播放全部

    void playMusicByIndex(QString, int); //双击

private:
    Ui::RecentPage *ui;

    QString type;

    QVector<QString> musicListOfPage; // 具体某个⻚⾯的⾳乐，将来只需要存储⾳乐的id即可
};

#endif // RECENTPAGE_H
