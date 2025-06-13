#ifndef DOWNPAGE_H
#define DOWNPAGE_H

#include <QWidget>
#include <QMediaPlaylist>


#include "musiclist.h"

namespace Ui {
class DownPage;
}

class DownPage : public QWidget
{
    Q_OBJECT

public:
    explicit DownPage(QWidget *parent = nullptr);
    ~DownPage();

    QString typeName();

    void addMusicToMusicPage(MusicList &musicList);
    void reFresh(MusicList &musicList);

    void addMusicToPlayer(MusicList &musicList, QMediaPlaylist *playList);

signals:
    void updatalikeMusic(bool isLike, QString musicId);

    void playAll(QString type); //播放全部

    void playMusicByIndex(QString, int); //双击

private slots:
    void on_localSong_clicked();

private:
    Ui::DownPage *ui;
    QString type;

    QVector<QString> musicListOfPage; // 具体某个⻚⾯的⾳乐，将来只需要存储⾳乐的id即可
};

#endif // DOWNPAGE_H
