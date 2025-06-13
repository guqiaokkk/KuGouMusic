#include "commonpage.h"
#include "ui_commonpage.h"

#include "listitembox.h"
#include "downpage.h"

CommonPage::CommonPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommonPage),
    type("我喜欢")
{
    ui->setupUi(this);

    connect(ui->playAllBtn, &QPushButton::clicked, this,[=](){
            emit playAll(type);
    });

    connect(ui->pageMusicList, &QListWidget::doubleClicked, this, [=](const QModelIndex &index){
       // ⿏标双击后，发射信号告诉QQMusic，博能放this⻚⾯中共被双击的歌曲
        emit playMusicByIndex(this->typeName(), index.row());
    });

}

CommonPage::~CommonPage()
{
    delete ui;
}

void CommonPage::setCommonPageUi(const QString &title, const QString &imagePath)
{
    // 设置封⾯栏
    ui->musicImageLabel->setPixmap(QPixmap(imagePath));
    ui->musicImageLabel->setScaledContents(true);

    //设置标题
    ui->pageTittle->setText(title);



    ui->pageMusicList->setStyleSheet("QListWidget::item:hover { background: transparent; }");
}

QString CommonPage::typeName()
{
    return type;
}

void CommonPage::addMusicToMusicPage(MusicList &musicList)
{
    // 将旧内容清空
    musicListOfPage.clear();
    for(auto &music : musicList)
    {
        if(music.getIsLike())
        {
            musicListOfPage.push_back(music.getMusicId());
        }
    }
}

void CommonPage::reFresh(MusicList &musicList)
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

        ListItemBox *listItemBox = new ListItemBox(this);
        listItemBox->setMusicName(it->getMusicName());
        listItemBox->setSinger(it->getSingerName());
        listItemBox->setAlbumName(it->getAlbumName());
        listItemBox->setLikeIcon(it->getIsLike());

        QListWidgetItem *item = new QListWidgetItem(ui->pageMusicList);
        item->setSizeHint(QSize(listItemBox->width(), listItemBox->height()));
        ui->pageMusicList->setItemWidget(item, listItemBox);

        // 接收ListItemBox发射的setIsLike信号
        connect(listItemBox, &ListItemBox::setIsLike, this, [=](bool isLike){
           emit updatalikeMusic(isLike, it->getMusicId());
        });
    }
}

void CommonPage::addMusicToPlayer(MusicList &musicList, QMediaPlaylist *playList)
{
    // 根据当前的⻚⾯,将⾳乐添加到playList中
    for(auto music : musicList)
    {
        if(music.getIsLike())
        {
            playList->addMedia(music.getMusicUrl());

        }
    }
}
