#include "listitembox.h"
#include "ui_listitembox.h"
#include <QDebug>

ListItemBox::ListItemBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItemBox),
    isLike(false)
{
    ui->setupUi(this);


    // 你手动添加过一次槽函数，唉，以后多注意一些，如果你有一些额外的个人行为，会导致问题就很玄学
    // likeBtn按钮连接其点击槽函数
    //    connect(ui->likeBtn, &QPushButton::clicked, this, &ListItemBox::on_likeBtn_clicked);// 这里
}

ListItemBox::~ListItemBox()
{
    delete ui;
}

void ListItemBox::enterEvent(QEvent *event)
{
    (void)event;
    setStyleSheet("background-color: #EFEFEF;");
}

void ListItemBox::leaveEvent(QEvent *event)
{
    (void)event;
    setStyleSheet("");
}

void ListItemBox::setMusicName(const QString& name)
{
    ui->musicNameLabel->setText(name);
}

void ListItemBox::setSinger(const QString& singer)
{
    ui->musicSingerLabel->setText(singer);
}

void ListItemBox::setAlbumName(const QString& albumName)
{
    ui->musicAlbumLabel->setText(albumName);
}

void ListItemBox::setLikeIcon(bool like)
{
    isLike = like;
    if(isLike)
    {
        ui->likeBtn->setIcon(QIcon(":/Image/redlike.png"));
    }
    else
    {
        ui->likeBtn->setIcon(QIcon(":/Image/dislike.png"));
    }
}



void ListItemBox::on_likeBtn_clicked()
{
    //qDebug() << "isLike like: " << isLike;
    isLike = !isLike;
    setLikeIcon(isLike);

    // 现在收藏或不收藏的状态改变,应该通知修改Music对象中的isLike属性
    emit setIsLike(isLike);
}
