#ifndef KUGOUMUSIC_H
#define KUGOUMUSIC_H
#include "volumetool.h"
#include "musiclist.h"
#include "lrcpage.h"


#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPropertyAnimation>

#include <QSqlDatabase>

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

    void initSQLite();

    void initMusicList();

private:
    void updatePlayingBtFormAnimation();

private slots:
    void on_quit_clicked();

    void onBtFromClick(int pageId);

    void on_volume_clicked();

    void on_addLocal_clicked();

    void playAllOfPage(QString type, int index); //根据type来播放对于type页面的所有歌曲
    void playMusicByIndex(QString type, int index);

    void onPlayCliked();// 播放按钮
    void onPlayStateChanged();

    void onPlaybackModeCliked();//播放模式设置

    void onPlayUpClicked();  //上一首
    void onPlayDownClicked();//下一首

    void setMusicSilence(bool isMuted);// VolumeTool类中setMusicMuted信号对应的槽函数

    void setPlayerVolume(int volume);// 设置⾳量⼤⼩

    void onPlaybackModeChanged(QMediaPlaylist::PlaybackMode playbackMode);// 播放模式切换槽函数

    void onCurrentIndexChanged(int index);//实现最近播放

    void onDurationChanged(qint64 duration);// 歌曲持续时⻓改变时[歌曲切换]

    void onPositionChanged(qint64 duration);// 播放位置改变，即持续播放时间改变

    // 进度条相关的槽函数
    void onSliderPositionChanged(float value);
    void onSliderPressed();
    void onSliderReleased();


    void onMetaDataAvailableChangedChanged(bool available); //当歌曲更换时，完成信息的更新

    void onLrcWordClicked();



    void on_min_clicked();

    void on_max_clicked();

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

    QString nowType; //标识当前页面

    Music *currentMusic;//当前歌曲

    qint64 totalTime;  // 记录媒体源的总时间
    bool isSliderDragging; // 记录用户是否正在拖拽进度条

    LrcPage *lrcPage; //歌词界面
    QPropertyAnimation *lrcAnimation; //歌词界面相关的动画

    QSqlDatabase sqlite;

    bool isDrag; // 为true时候窗⼝才能拖拽

};
#endif // KUGOUMUSIC_H
