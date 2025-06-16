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
#include <QMessageBox>
#include <QSqlQuery>


KuGouMusic::KuGouMusic(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KuGouMusic)
    , isSliderDragging(false)
{
    ui->setupUi(this);
    initUI();

    // 初始化数据库
    initSQLite();

    // 加载数据库歌曲⽂件
    initMusicList();

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

    // 设置主窗⼝图标
    setWindowIcon(QIcon(":/Image/KuGou.png"));

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
    ui->stackedWidget->setCurrentIndex(4);
    nowType = "本地音乐";

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

    // 创建lrc歌词窗⼝
    lrcPage = new LrcPage(this);
    lrcPage->hide();

    // lrcPage添加动画效果
    lrcAnimation = new QPropertyAnimation(lrcPage, "geometry", this);
    lrcAnimation->setDuration(500);
    lrcAnimation->setStartValue(QRect(15, 15 + lrcPage->height(), lrcPage->width(), lrcPage->height()));
    lrcAnimation->setEndValue(QRect(15, 15, lrcPage->width(), lrcPage->height()));
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
    connect(ui->recentPage, &RecentPage::updatalikeMusic, this, &KuGouMusic::onUpdateLikeMusic);

    //暂停，启动
    connect(ui->play, &QPushButton::clicked, this, &KuGouMusic::onPlayCliked);

    //上下首
    connect(ui->playPrev, &QPushButton::clicked, this, &KuGouMusic::onPlayUpClicked);
    connect(ui->playDown, &QPushButton::clicked, this, &KuGouMusic::onPlayDownClicked);

    // 设置播放模式
    connect(ui->playMode, &QPushButton::clicked, this, &KuGouMusic::onPlaybackModeCliked);

    //不同页面的playAll
    connect(ui->likePage, &CommonPage::playAll, this, [=](){
        QString tmp = ui->likePage->typeName();
        playAllOfPage(tmp, 0);
    });
    connect(ui->localPage, &DownPage::playAll, this, [=](){
        QString tmp = ui->localPage->typeName();
        playAllOfPage(tmp, 0);
    });
    connect(ui->recentPage, &RecentPage::playAll, this, [=](){
        QString tmp = ui->recentPage->typeName();
        playAllOfPage(tmp, 0);
    });

    //处理不同页面的双击
    connect(ui->likePage, &CommonPage::playMusicByIndex, this, &KuGouMusic::playMusicByIndex);
    connect(ui->localPage, &DownPage::playMusicByIndex, this, &KuGouMusic::playMusicByIndex);
    connect(ui->recentPage, &RecentPage::playMusicByIndex, this, &KuGouMusic::playMusicByIndex);

    // 设置静⾳
    connect(volumeTool, &VolumeTool::setSilence, this, &KuGouMusic::setMusicSilence);
    // 设置⾳量⼤⼩
    connect(volumeTool, &VolumeTool::setMusicVolume, this, &KuGouMusic::setPlayerVolume);

    // 媒体持续时⻓更新，即：⾳乐切换，时⻓更新，界⾯上时间也要更新
    connect(player, &QMediaPlayer::durationChanged, this, &KuGouMusic::onDurationChanged);

    // 播放位置发⽣改变，即已经播放时间更新
    connect(player, &QMediaPlayer::positionChanged, this, &KuGouMusic::onPositionChanged);

    //当playlist中播放源发生变化时
    connect(player, &QMediaPlayer::metaDataAvailableChanged, this, &KuGouMusic::onMetaDataAvailableChangedChanged);

    //进度条相关连接
    // 1. 当 MusicSlider 报告位置变化时（仅在鼠标释放后），我们去改变播放器位置
    connect(ui->ProcessBar, &MusicSlider::positionChanged, this, &KuGouMusic::onSliderPositionChanged);
    // 2. 当 MusicSlider 报告被按下时，我们记录下拖拽状态
    connect(ui->ProcessBar, &MusicSlider::sliderPressed, this, &KuGouMusic::onSliderPressed);
    // 3. 当 MusicSlider 报告被释放时，我们也记录下拖拽状态
    connect(ui->ProcessBar, &MusicSlider::sliderReleased, this, &KuGouMusic::onSliderReleased);

    // 显⽰歌词窗⼝
    connect(ui->lrcWord, &QPushButton::clicked, this, &KuGouMusic::onLrcWordClicked);
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
    ui->recentPage->reFresh(musicList);
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

    // 播放列表项发⽣改变，此时将播放⾳乐收藏到历史记录中
    connect(playlist, &QMediaPlaylist::currentIndexChanged, this, &KuGouMusic::onCurrentIndexChanged);
}

void KuGouMusic::initSQLite()
{
    // 1. 创建数据库连接
    sqlite = QSqlDatabase::addDatabase("QSQLITE");

    // 2. 设置数据库名称
    sqlite.setDatabaseName("KuGou.db");

    // 3. 打开数据库
    if(!sqlite.open())
    {
        QMessageBox::critical(this, "KuGouMusic", "数据库打开失败！");
        return;
    }
    qDebug()<<"SQLite连接成功，并创建 [KuGou.db] 数据库!!!";

    // 4. 创建数据库表
    QString sql = ("CREATE TABLE IF NOT EXISTS musicInfo(\
                    id INTEGER PRIMARY KEY AUTOINCREMENT,\
                    musicId varchar(200) UNIQUE,\
                    musicName varchar(50),\
                    singerName varchar(50),\
                    albumName varchar(50),\
                    duration BIGINT,\
                    musicUrl varchar(256),\
                    lrcPath varchar(256),\
                    isLike INTEGER,\
                    isHistory INTEGER)");

    QSqlQuery query;
    if(!query.exec(sql))
    {
        QMessageBox::critical(this, "KuGouMusic", "初始化错误！！！");
        return;
    }
    qDebug()<<"创建 KuGou表成功!!!";

    //检查并添加 lrcPath 字段
    if (!query.exec("SELECT lrcPath FROM musicInfo LIMIT 1"))
    {

        if (!query.exec("ALTER TABLE musicInfo ADD COLUMN lrcPath varchar(256)")) {
               QMessageBox::critical(this, "KuGouMusic", "lrcPath字段出错");
            }
        else
        {
                qDebug() << "lrc文件地址添加成功！";
        }
     }
}

void KuGouMusic::initMusicList()
{
    // 1. 从数据库读取歌曲信息
    musicList.readFromDB();

    // 2. 更新⻚⾯
    ui->likePage->reFresh(musicList);
    ui->localPage->reFresh(musicList);
    ui->recentPage->reFresh(musicList);
}

void KuGouMusic::updatePlayingBtFormAnimation()
{
    // 1. 获取所有的 btForm 控件
    QList<btForm*> allBtForms = this->findChildren<btForm*>();

    // 2. 遍历所有 btForm
    for(btForm* form : allBtForms)
    {
        // 3. 根据 nowType 判断是否是当前正在播放的列表对应的 btForm
        bool shouldShow = false;
        switch(form->getId())
        {
          case 3: if (nowType == "我喜欢") shouldShow = true; break;// 我喜欢
          case 4: if (nowType == "本地音乐") shouldShow = true;break;// 本地音乐
          case 5: if (nowType == "最近播放") shouldShow = true;break;// 最近播放
          // 根据需要添加其他页面的 case
          default:break;
         }
         // 4. 根据判断结果，显示或隐藏动画
         if (shouldShow)
         {
            form->showAnimation();
         }
         else
         {
            form->hideAnimation();
         }
    }
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
    QList<btForm*> allBtForms = this->findChildren<btForm*>();
    if(player->state() == QMediaPlayer::PlayingState)
    {
        // 播放状态
        ui->play->setIcon(QIcon(":/Image/stop.png"));
        // 开始播放时，确保动画是正确的状态
        updatePlayingBtFormAnimation();
    }
    else if (player->state() == QMediaPlayer::PausedState)
    {
        // 暂停状态
        ui->play->setIcon(QIcon(":/Image/musicstop.png"));

        // 遍历所有btForm，并暂停它们的动画
        for(btForm* form : allBtForms)
        {
            form->pauseAnimation();
        }
    }
    else // StoppedState
    {
         // 停止状态
         ui->play->setIcon(QIcon(":/Image/musicstop.png"));
         // 停止时，我们希望所有动画都消失
         for (btForm* form : allBtForms)
         {
            form->hideAnimation();
         }
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
        nowType = "我喜欢";
        ui->likePage->addMusicToPlayer(musicList, playlist);
    }
    else if(type == "本地音乐")
    {
        nowType = "本地音乐";
        ui->localPage->addMusicToPlayer(musicList, playlist);
    }
    else if(type == "最近播放")
    {
        nowType = "最近播放";
        ui->recentPage->addMusicToPlayer(musicList, playlist);
    }
    else
    {
        nowType = "";
    }

    // 在改变了 nowType 之后，立即更新动画状态
    updatePlayingBtFormAnimation();

    playlist->setCurrentIndex(index);

    player->play();
}

void KuGouMusic::playMusicByIndex(QString type, int index)
{
    playAllOfPage(type, index);
}

void KuGouMusic::onCurrentIndexChanged(int index)
{
    if(index < 0)
    {
        return;
    }

    //从播放列表中获取媒体内容
    QMediaContent content = playlist->media(index);
    if(content.isNull()){
        return;
    }

    //通过 URL 查找歌曲
    currentMusic = musicList.findMusicByUrl(content.canonicalUrl());

    //标记歌曲为历史记录
    if(currentMusic != nullptr)
    {
        currentMusic->setIsHistory(true);
    }

    //刷新“最近播放”页面
    ui->recentPage->reFresh(musicList);
}

void KuGouMusic::setMusicSilence(bool isMuted)
{
    player->setMuted(isMuted);
}

void KuGouMusic::setPlayerVolume(int volume)
{
    player->setVolume(volume);
}

void KuGouMusic::onDurationChanged(qint64 duration)
{
    // duration为歌曲的总时长,单位为毫秒
    totalTime = duration;

    ui->totalTime->setText(QString("%1:%2").arg(duration/1000/60, 2, 10, QChar('0'))
                                             .arg(duration/1000%60, 2, 10, QChar('0')));
}

void KuGouMusic::onPositionChanged(qint64 duration)
{

    // 如果用户正在拖拽，则忽略来自播放器的所有UI更新请求
        if (isSliderDragging) {
            return;
        }
    // 1. 更新当前播放时间
    ui->currentTime->setText(QString("%1:%2").arg(duration/1000/60, 2, 10, QChar('0'))
                             .arg(duration/1000%60, 2, 10, QChar('0')));

    // 2. 更新进度条的位置
    if (totalTime > 0)
    {
       float percent = (float)duration / totalTime;
       ui->ProcessBar->setPosition(percent);
    }
    else
    {
       ui->ProcessBar->setPosition(0.0f);
    }

    // 3. 同步lrc歌词
    if(currentMusic)
    {
        lrcPage->showLrcWord(duration);
    }
}

void KuGouMusic::onSliderPositionChanged(float value)
{
    // 此函数只在鼠标释放后被调用一次，所以可以安全地设置播放器位置
    if (totalTime > 0)
    {
         qint64 position = static_cast<qint64>(totalTime * value);
         player->setPosition(position);
    }
}

void KuGouMusic::onSliderPressed()
{
     isSliderDragging = true;
}

void KuGouMusic::onSliderReleased()
{
    isSliderDragging = false;
}


void KuGouMusic::onMetaDataAvailableChangedChanged(bool available)
{
    // 播放源改变
     qDebug()<<"歌曲切换";

     // 1. 从curretnMusic中获取歌曲信息
     QString singer = currentMusic->getSingerName();
     QString musicName = currentMusic->getMusicName();


     // 2. 设置歌⼿、歌曲名称、专辑名称
     ui->MusicName->setText(musicName);
     ui->MusicSinger->setText(singer);

     // 3. 获取封⾯图⽚
     QVariant coverImage = player->metaData("ThumbnailImage");
     if(coverImage.isValid())
     {
         // 获取封⾯图⽚成功
         QImage image = coverImage.value<QImage>();

         ui->MusicImage->setPixmap(QPixmap::fromImage(image));

         // 缩放填充到整个Label
         ui->MusicImage->setScaledContents(true);

         QPixmap coverPixmap = QPixmap::fromImage(image);
         lrcPage->setAlbumCover(coverPixmap);

         if(currentMusic->getIsLike())
         {
             ui->likePage->setImageLabel(QPixmap::fromImage(image));
         }
     }
     else
     {
         // coverImage 无效，或者转换出的图片为空。
         // 这意味着当前播放的歌曲没有内嵌封面
         qDebug() << "检测到新歌曲没有封面，设置为默认图片。";
         QPixmap defaultPixmap(":/Image/KuGou.png");
         ui->MusicImage->setPixmap(defaultPixmap);
         ui->MusicImage->setScaledContents(true);

         lrcPage->setAlbumCover(defaultPixmap);

        // 将“我喜欢”页面的封面重置为歌单的默认封面
        if (currentMusic->getIsLike())
        {
          // 重置"我喜欢" 页面
          ui->likePage->setCommonPageUi("我喜欢", ":/Image/KuGou.png");
        }
     }

     // 加载lrc歌词并解析
     if(currentMusic)
     {
         lrcPage->parseLrc(currentMusic->getLrcPath());
     }
}

void KuGouMusic::onLrcWordClicked()
{
    // 显⽰窗⼝ 并 开启动画
    lrcPage->show();
    lrcAnimation->start();
}


void KuGouMusic::on_quit_clicked()
{
    // 更新数据库
    musicList.writeToDB();

    // 关闭数据库连接
    sqlite.close();

    // 关闭窗⼝
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

    // 点击btForm时，窗⼝不能拖拽
    isDrag = false;

}

void KuGouMusic::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton )
    {
        isDrag = true;
        //计算鼠标按下时，鼠标相对于窗口左上角的偏移量
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        return;
    }
    QWidget::mousePressEvent(event);
}

void KuGouMusic::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton && isDrag)
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

void KuGouMusic::on_min_clicked()
{
    showMinimized();
}

void KuGouMusic::on_max_clicked()
{
   QMessageBox::information(this, "提示", "正在加班开发中qwq....");
}
