#ifndef KUGOUMUSIC_H
#define KUGOUMUSIC_H
#include "volumetool.h"
#include "musiclist.h"



#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>

QT_BEGIN_NAMESPACE
namespace Ui { class KuGouMusic; }
QT_END_NAMESPACE

class KuGouMusic : public QWidget
{
    Q_OBJECT

public:
    KuGouMusic(QWidget *parent = nullptr);
    ~KuGouMusic();

    void initUI();

    void connectSignalAndSlots();

    void onUpdateLikeMusic(bool isLike, QString musicId);

    void initPlayer(); //初始化媒体对象

    void onPlayCliked();// 播放按钮
    void onPlayStateChanged();

    void onPlayUpClicked();  //上一首
    void onPlayDownClicked();//下一首

    void onPlaybackModeCliked();//播放模式设置

    void onPlaybackModeChanged(QMediaPlaylist::PlaybackMode playbackMode);// 播放模式切换槽函数

    void playAllOfPage(QString type, int index);

    void playMusicByIndex(QString type, int index);

private slots:
    void on_quit_clicked();

    void onBtFromClick(int pageId);

    void on_volume_clicked();

    void on_addLocal_clicked();


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    QJsonArray RandomPicture();

private:
    Ui::KuGouMusic *ui;
    QPoint dragPosition;

    VolumeTool *volumeTool;

    MusicList musicList;

    QMediaPlayer *player; // 播放器相关
    QMediaPlaylist *playlist; //给播放器设置媒体列表
};
#endif // KUGOUMUSIC_H
