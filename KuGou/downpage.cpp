#include "downpage.h"
#include "ui_downpage.h"

#include "listitembox.h"

#include <QDebug>


DownPage::DownPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownPage),
    type("本地音乐")
{
    ui->setupUi(this);
    connect(ui->localSong, &QPushButton::clicked, this, [=](){
            ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->downLoadSong, &QPushButton::clicked, this, [=](){
            ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->downLoadVideo, &QPushButton::clicked, this, [=](){
            ui->stackedWidget->setCurrentIndex(2);
    });
    connect(ui->downLoading, &QPushButton::clicked, this, [=](){
            ui->stackedWidget->setCurrentIndex(3);
    });

    connect(ui->playAllBtn, &QPushButton::clicked, this,[=](){
            emit playAll(type);
    });

    connect(ui->pageMusicList, &QListWidget::doubleClicked, this, [=](const QModelIndex &index){
       // ⿏标双击后，发射信号告诉QQMusic，博能放this⻚⾯中共被双击的歌曲
        emit playMusicByIndex(this->typeName(), index.row());
    });
}

DownPage::~DownPage()
{
    delete ui;
}

QString DownPage::typeName()
{
    return type;
}

void DownPage::addMusicToMusicPage(MusicList &musicList)
{
    // 将旧内容清空
    musicListOfPage.clear();
    for(auto &music : musicList)
    {
            musicListOfPage.push_back(music.getMusicId());
    }
}

void DownPage::reFresh(MusicList &musicList)
{
    ui->pageMusicList->clear();
    // 从musicList中分离出当前⻚⾯的所有⾳乐
    addMusicToMusicPage(musicList);

    // 遍历歌单，将歌单中的歌曲显⽰到界⾯
    for(auto musicId : musicListOfPage)
    {
        auto it = musicList.findMusicById(musicId);
        if(it == musicList.end())
        {
            continue;
        }

        ListItemBox *listItemBox = new ListItemBox(ui->pageMusicList);
        listItemBox->setMusicName(it->getMusicName());
        listItemBox->setSinger(it->getSingerName());
        listItemBox->setAlbumName(it->getAlbumName());
        listItemBox->setLikeIcon(it->getIsLike());

        QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->pageMusicList);
        listWidgetItem->setSizeHint(QSize(ui->pageMusicList->width(), 45));
        ui->pageMusicList->setItemWidget(listWidgetItem, listItemBox);

        // 接收ListItemBox发射的setIsLike信号
        connect(listItemBox, &ListItemBox::setIsLike, this, [=](bool isLike){
           emit updatalikeMusic(isLike, it->getMusicId());
        });
    }

    //刷新
    repaint();
}

void DownPage::addMusicToPlayer(MusicList &musicList, QMediaPlaylist *playList)
{
    // 根据当前的⻚⾯,将⾳乐添加到playList中
    for(auto music : musicList)
    {
        playList->addMedia(music.getMusicUrl());
    }
}


void DownPage::on_localSong_clicked()
{
    qDebug() << "jiayou";
}
