#include "recboxitem.h"
#include "ui_recboxitem.h"
#include <QPropertyAnimation>
#include <QDebug>

RecBoxItem::RecBoxItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBoxItem)
{
    ui->setupUi(this);
    ui->musicImageBox->installEventFilter(this);
}

RecBoxItem::~RecBoxItem()
{
    delete ui;
}

bool RecBoxItem::eventFilter(QObject *watched, QEvent *event)
{
    //recItem上有⼀个按钮，当⿏标放在按钮上时在开启动画
    if(watched == ui->musicImageBox)
    {
      int ImgWidth = ui->musicImageBox->width();
      int ImgHeight = ui->musicImageBox->height();

      // 拦截⿏标进⼊事件
      if(event->type() == QEvent::Enter)
      {
          QPropertyAnimation *animation = new QPropertyAnimation(ui->musicImageBox, "geometry");
          animation->setDuration(100);
          animation->setStartValue(QRect(10, 10, ImgWidth, ImgHeight));
          animation->setEndValue(QRect(10, 0, ImgWidth, ImgHeight));
          animation->start();
          //动画结束的时候会触发finished信号，拦截到该信号，销毁animation
          connect(animation, &QPropertyAnimation::finished, this,
                  [=](){delete animation;
                        qDebug() << "图片上移结束";});
          return true;
      }
      else if(event->type() == QEvent::Leave)
      {
          // 拦截⿏标离开事件
          QPropertyAnimation *animation = new QPropertyAnimation(ui->musicImageBox, "geometry");
          animation->setDuration(150);
          animation->setStartValue(QRect(10, 0, ImgWidth, ImgHeight));
          animation->setEndValue(QRect(10, 10, ImgWidth, ImgHeight));
          animation->start();
          //动画结束的时候会触发finished信号，拦截到该信号，销毁animation
          connect(animation, &QPropertyAnimation::finished, this,
                  [=](){delete animation;
                        qDebug() << "图片下移结束";});
          return true;
      }
    }
    return QObject::eventFilter(watched, event);
}

void RecBoxItem::setText(const QString &text)
{
    ui->recBoxItemText->setText(text);
}

void RecBoxItem::setImage(const QString &Imagepath)
{
    QString imgStyle = "border-image:url(" + Imagepath +")";
    ui->recMusicImage->setStyleSheet(imgStyle);
}
