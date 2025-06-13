#include "kugoumusic.h"
#include "ui_kugoumusic.h"


#include <QDebug>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QFileDialog>

KuGouMusic::KuGouMusic(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KuGouMusic)
{
    ui->setupUi(this);
    initUI();

    //初始化播放器
    initPlayer();

    connectSignalAndSlots();
}

KuGouMusic::~KuGouMusic()
{
    delete ui;
}

void KuGouMusic::initUI()
{
    this->setWindowFlag(Qt::WindowType::FramelessWindowHint);

    // 设置窗⼝背景透明
    setAttribute(Qt::WA_TranslucentBackground);

    // 给窗⼝设置阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);           // 设置阴影偏移
    shadowEffect->setColor("#000000");       // 设置阴影颜⾊：⿊⾊
    shadowEffect->setBlurRadius(10);         // 设置阴影的模糊半径
    ui->background->setGraphicsEffect(shadowEffect);

    // 设置BtForm图标 & 文本信息
    ui->rec->setIconAndText(":/Image/rec.png", "推荐", 0);
    ui->radio->setIconAndText(":/Image/radio.png", "电台", 1);
    ui->music->setIconAndText(":/Image/music.png", "音乐馆", 2);
    ui->like->setIconAndText(":/Image/like.png", "我喜欢", 3);
    ui->local->setIconAndText(":/Image/local.png", "本地下载", 4);
    ui->recent->setIconAndText(":/Image/recent.png", "最近播放", 5);

    ui->like->showAnimation();
    ui->stackedWidget->setCurrentIndex(0);

    //RandomPicture();
    srand(time(NULL));
    ui->recMusicBox->initRecBoxUi(RandomPicture(), 1);
    ui->supplyMusicBox->initRecBoxUi(RandomPicture(), 2);

    //LikePage
    ui->likePage->setCommonPageUi("我喜欢", ":/Image/KuGou.png");

    //创建⾳量调节窗⼝对象并挂到对象树
    volumeTool = new VolumeTool(this);

    ui->play->setIcon(QIcon(":/Image/musicstop.png"));  // 默认为暂停图标
    ui->playMode->setIcon(QIcon(":/Image/shuffle.png"));// 默认为随机播放
}

void KuGouMusic::connectSignalAndSlots()
{
    connect(ui->rec, &btForm::btClick, this, &KuGouMusic::onBtFromClick);
    connect(ui->music, &btForm::btClick, this, &KuGouMusic::onBtFromClick);
    connect(ui->radio, &btForm::btClick, this, &KuGouMusic::onBtFromClick);
    connect(ui->like, &btForm::btClick, this, &KuGouMusic::onBtFromClick);
    connect(ui->local, &btForm::btClick, this, &KuGouMusic::onBtFromClick);
    connect(ui->recent, &btForm::btClick, this, &KuGouMusic::onBtFromClick);

    // 关联各个Page发射的updateLikeMusic信号
    //connect(ui->likePage, &CommonPage::updatalikeMusic, ui->localPage, &DownPage::updatalikeMusic);
    connect(ui->likePage, &CommonPage::updatalikeMusic, this, &KuGouMusic::onUpdateLikeMusic);
    connect(ui->localPage, &DownPage::updatalikeMusic, this, &KuGouMusic::onUpdateLikeMusic);
    //connect(ui->likePage, &RecentPage::updatalikeMusic, this, &KuGouMusic::onUpdateLikeMusic);

    //暂停，启动
    connect(ui->play, &QPushButton::clicked, this, &KuGouMusic::onPlayCliked);

    //上下首
    connect(ui->playPrev, &QPushButton::clicked, this, &KuGouMusic::onPlayUpClicked);
    connect(ui->playDown, &QPushButton::clicked, this, &KuGouMusic::onPlayDownClicked);

    // 设置播放模式
    connect(ui->playMode, &QPushButton::clicked, this, &KuGouMusic::onPlaybackModeCliked);

    //不同页面的playAll
    connect(ui->likePage, &CommonPage::playAll, this, [=](){
        QString tmp = ui->localPage->typeName();
        playAllOfPage(tmp, 0);
    });
    connect(ui->localPage, &DownPage::playAll, this, [=](){
        QString tmp = ui->localPage->typeName();
        playAllOfPage(tmp, 0);
    });

    //处理不同页面的双击
    connect(ui->likePage, &CommonPage::playMusicByIndex, this, &KuGouMusic::playMusicByIndex);
    connect(ui->localPage, &DownPage::playMusicByIndex, this, &KuGouMusic::playMusicByIndex);
}

void KuGouMusic::onUpdateLikeMusic(bool isLike, QString musicId)
{
    // 1. 找到该⾸歌曲，并更新对应Music对象信息
    auto it = musicList.findMusicById(musicId);
    if(it != musicList.end())
    {
        it->setIsLike(isLike);
    }
    ui->likePage->reFresh(musicList);
    ui->localPage->reFresh(musicList);
    //ui->recentPage->reFresh();
}

void KuGouMusic::initPlayer()
{
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);

    //设置播放模式：默认为随机播放
    playlist->setPlaybackMode(QMediaPlaylist::Random);

    // 将播放列表设置给播放器
    player->setPlaylist(playlist);

    //默认音量
    player->setVolume(20);

    // QMediaPlayer信号和槽函数关联  播放状态改变时：暂停和播放之间切换
    connect(player, &QMediaPlayer::stateChanged, this, &KuGouMusic::onPlayStateChanged);

    // 播放列表的模式放⽣改变时的信号槽关联
    connect(playlist, &QMediaPlaylist::playbackModeChanged, this, &KuGouMusic::onPlaybackModeChanged);
}

void KuGouMusic::onPlayCliked()
{
    if(player->state() == QMediaPlayer::PlayingState)
    {
        // 如果是歌曲正在播放中，按下播放键，此时应该暂停播放
        player->pause();
    }
    else if(player->state() == QMediaPlayer::PausedState)
    {
        // 如果是暂停状态，按下播放键，继续开始播放
        player->play();
    }
    else if(player->state() == QMediaPlayer::StoppedState)
    {
        player->play();
    }

}

void KuGouMusic::onPlayStateChanged()
{
    if(player->state() == QMediaPlayer::PlayingState)
    {
        // 播放状态
        ui->play->setIcon(QIcon(":/Image/stop.png"));
    }
    else
    {
        // 暂停状态
        ui->play->setIcon(QIcon(":/Image/musicstop.png"));
    }
}

void KuGouMusic::onPlayUpClicked()
{
    playlist->previous();
}

void KuGouMusic::onPlayDownClicked()
{
    playlist->next();
}

void KuGouMusic::onPlaybackModeCliked()
{
    // 播放模式⽀持：循环播放、随机播放、单曲循环三种模式
    if(playlist->playbackMode() == QMediaPlaylist::Loop)
    {
        //列表循环
        ui->playMode->setToolTip("随机播放");
        playlist->setPlaybackMode(QMediaPlaylist::Random);
    }
    else if(playlist->playbackMode() == QMediaPlaylist::Random)
    {
        //随机播放
        ui->playMode->setToolTip("单曲循环");
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else if(playlist->playbackMode() == QMediaPlaylist::CurrentItemInLoop)
    {
        ui->playMode->setToolTip("列表循环");
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
    else
    {
        qDebug() << "未识别的播放模式，暂不支持";
    }
}

void KuGouMusic::onPlaybackModeChanged(QMediaPlaylist::PlaybackMode playbackMode)
{
    if(playbackMode == QMediaPlaylist::Loop)
    {
        ui->playMode->setIcon(QIcon(":/Image/listSong.png"));
    }
    else if(playbackMode == QMediaPlaylist::Random)
    {
        ui->playMode->setIcon(QIcon(":/Image/shuffle.png"));
    }
    else if(playbackMode == QMediaPlaylist::CurrentItemInLoop)
    {
        ui->playMode->setIcon(QIcon(":/Image/oneSong.png"));
    }
    else
    {
        qDebug() << "此模式暂时不支持";
    }
}



void KuGouMusic::playAllOfPage(QString type, int index)
{
    playlist->clear();

    if(type == "我喜欢")
    {
        ui->likePage->addMusicToPlayer(musicList, playlist);
    }
    else if(type == "本地音乐")
    {
        ui->localPage->addMusicToPlayer(musicList, playlist);
    }

    playlist->setCurrentIndex(index);

    player->play();
}

void KuGouMusic::playMusicByIndex(QString type, int index)
{
    playAllOfPage(type, index);
}


void KuGouMusic::on_quit_clicked()
{
    close();
}

void KuGouMusic::onBtFromClick(int pageId)
{
    // 清除之前btForm按钮的颜色背景
    // 获取所有的BtForm按钮
    QList<btForm*> tmplist = this->findChildren<btForm*>();
    for(auto btFrom : tmplist)
    {
        if(btFrom->getId() != pageId)
        {
            btFrom->clearbg();
        }
    }

    ui->stackedWidget->setCurrentIndex(pageId);
    qDebug() << "change page";

}

void KuGouMusic::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        //计算鼠标按下时，鼠标相对于窗口左上角的偏移量
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        return;
    }
    QWidget::mousePressEvent(event);
}

void KuGouMusic::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        // 根据鼠标移动的位置，和之前记录的偏移量 dragPosition，计算窗口的新位置并移动窗口
        // 计算出窗口左上角的新位置，使得鼠标与窗口左上角始终保持相同的相对位置
        move(event->globalPos() - dragPosition);
        return;
    }
    QWidget::mouseMoveEvent(event);
}

QJsonArray KuGouMusic::RandomPicture()
{
    // 推荐⽂本 + 推荐图⽚路径
    QVector<QString> vecImageName;
    vecImageName << "001.png"<< "002.png" <<"003.png"<<"004.png"<<"005.png"<<"006.png"
    <<"007.png"
     <<"008.png"<<"009.png"<<"010.png"<<"011.png"<<"012.png"
    <<"013.png"
     <<"014.png"<<"015.png"<<"016.png"<<"017.png"<<"018.png"
    <<"019.png"
     <<"020.png"<<"021.png"<<"022.png"<<"023.png"<<"024.png"
    <<"025.png"
     <<"026.png"<<"027.png"<<"028.png"<<"029.png"<<"030.png"
    <<"031.png"
     <<"032.png"<<"033.png"<<"034.png"<<"035.png"<<"036.png"
    <<"037.png"
     <<"038.png"<<"039.png"<<"040.png";

    std::random_shuffle(vecImageName.begin(), vecImageName.end());

    QJsonArray objArray;
    for(int i = 0; i < vecImageName.size(); i++)
    {
        QJsonObject obj;
        obj.insert("path", ":/Image/rec/" + vecImageName[i]);
        QString strText = QString("推荐-%1").arg(i, 3, 10, QChar('0'));
        obj.insert("text", strText);

        objArray.append(obj);
    }
    return objArray;
}

void KuGouMusic::on_volume_clicked()
{
    QPoint point = ui->volume->mapToGlobal(QPoint(0, 0));

    QPoint volumeLeftTop = point - QPoint(volumeTool->width()/2, volumeTool->height());

    volumeLeftTop.setX(volumeLeftTop.x() + 15);
    volumeLeftTop.setY(volumeLeftTop.y() + 15);

    volumeTool->move(volumeLeftTop);

    volumeTool->show();

}

void KuGouMusic::on_addLocal_clicked()
{
    // 1. 创建⼀个⽂件对话框
    QFileDialog fileDialog(this);
    fileDialog.setWindowTitle("添加本地音乐");

    // 2. 创建⼀个打开格式的⽂件对话框
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    // 3. 设置对话框模式 只能选择⽂件，并且⼀次性可以选择多个存在的⽂件
    fileDialog.setFileMode(QFileDialog::ExistingFiles);

    // 4. 设置对话框的MIME过滤器
    QStringList mimeList;
    mimeList << "application/octet-stream";//这里实质上相当于没设置
    fileDialog.setMimeTypeFilters(mimeList);

    // 5. 设置对话框默认的打开路径,设置⽬录为当前⼯程所在⽬录
    QDir dir(QDir::currentPath());
    dir.cdUp();
    QString musicPath = dir.path() + "/Music/";
    fileDialog.setDirectory(musicPath);

    // 6. 显⽰对话框，并接收返回值
    // 模态对话框, exec内部是死循环处理
    if(fileDialog.exec() == QFileDialog::Accepted)
    {
        // 切换到本地⾳乐界⾯，因为加载完的⾳乐需要在本地⾳乐界⾯显⽰
        ui->stackedWidget->setCurrentIndex(4);

        // 获取对话框的返回值
        QList<QUrl> urls = fileDialog.selectedUrls();

        // 拿到歌曲⽂件后，将歌曲⽂件交由musicList进⾏管理
        musicList.addMusicByUrl(urls);


        // 更新到本地⾳乐列表
        ui->localPage->reFresh(musicList);

        ui->localPage->addMusicToPlayer(musicList, playlist);
    }
}
