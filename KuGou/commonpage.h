#ifndef COMMONPAGE_H
#define COMMONPAGE_H

#include "musiclist.h"

#include <QWidget>
#include <QMediaPlaylist>

namespace Ui {
class CommonPage;
}

class CommonPage : public QWidget
{
    Q_OBJECT

public:
    explicit CommonPage(QWidget *parent = nullptr);
    ~CommonPage();

    void setCommonPageUi(const QString &title, const QString &imagePath);

    QString typeName();

    void addMusicToMusicPage(MusicList &musicList);
    void reFresh(MusicList &musicList);

    void addMusicToPlayer(MusicList &musicList, QMediaPlaylist *playList);

signals:
    void updatalikeMusic(bool isLike, QString musicId);

    void playAll(QString type); //播放全部

    void playMusicByIndex(QString, int); //双击


private:
    Ui::CommonPage *ui;

    QString type;

    QVector<QString> musicListOfPage; // 具体某个⻚⾯的⾳乐，将来只需要存储⾳乐的id即可
};

#endif // COMMONPAGE_H
