#include "btform.h"
#include "ui_btform.h"

btForm::btForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::btForm)
{
    ui->setupUi(this);

    ui->lineBox->hide();
    QSizePolicy retain = ui->lineBox->sizePolicy();
    retain.setRetainSizeWhenHidden(true);
    ui->lineBox->setSizePolicy(retain);

    //设置line1
    animationLine1 = new QPropertyAnimation(ui->line1, "geometry", this);
    animationLine1->setDuration(1500);
    animationLine1->setKeyValueAt(0, QRect(0, 15, 2, 0));
    animationLine1->setKeyValueAt(0.5, QRect(0, 0, 2, 15));
    animationLine1->setKeyValueAt(1, QRect(0, 15, 2, 0));
    animationLine1->setLoopCount(-1);
    animationLine1->start();

    //设置line2
    animationLine2 = new QPropertyAnimation(ui->line2, "geometry", this);
    animationLine2->setDuration(1600);
    animationLine2->setKeyValueAt(0, QRect(4, 15, 2, 0));
    animationLine2->setKeyValueAt(0.5, QRect(4, 0, 2, 15));
    animationLine2->setKeyValueAt(1, QRect(4, 15, 2, 0));
    animationLine2->setLoopCount(-1);
    animationLine2->start();

    //设置line3
    animationLine3 = new QPropertyAnimation(ui->line3, "geometry", this);
    animationLine3->setDuration(1700);
    animationLine3->setKeyValueAt(0, QRect(8, 15, 2, 0));
    animationLine3->setKeyValueAt(0.5, QRect(8, 0, 2, 15));
    animationLine3->setKeyValueAt(1, QRect(8, 15, 2, 0));
    animationLine3->setLoopCount(-1);
    animationLine3->start();

    //设置line4
    animationLine4 = new QPropertyAnimation(ui->line4, "geometry", this);
    animationLine4->setDuration(1800);
    animationLine4->setKeyValueAt(0, QRect(12, 15, 2, 0));
    animationLine4->setKeyValueAt(0.5, QRect(12, 0, 2, 15));
    animationLine4->setKeyValueAt(1, QRect(12, 15, 2, 0));
    animationLine4->setLoopCount(-1);
    animationLine4->start();
}

btForm::~btForm()
{
    delete ui;
}

void btForm::setIconAndText(const QString &btIcon, const QString &btText, int pageId)
{
    ui->btIcon->setPixmap(QPixmap(btIcon));

    ui->btText->setText(btText);

    this->pageId = pageId;
}

void btForm::clearbg()
{
    ui->btStyle->setStyleSheet("#btStyle:hover{background:#D8D8D8;} "
                               "QLabel{background-color:transparent;}"
                               "#lineBox{background-color:transparent;}");


//    ui->btIcon->setAttribute(Qt::WA_TranslucentBackground);
//    ui->btText->setAttribute(Qt::WA_TranslucentBackground);
//    ui->btStyle->setStyleSheet("#btStyle:hover{background:#D8D8D8;} ");
}

int btForm::getId()
{
    return pageId;
}

void btForm::showAnimation()
{
    // showAnimation 现在也兼具 resume 的功能
    // 如果动画已经是运行状态，调用resume没效果，正好符合我们的需求
    ui->lineBox->show();
    resumeAnimation();
}

void btForm::hideAnimation()
{
    ui->lineBox->hide();

    // 停止动画会让它回到初始状态，这时隐藏
    animationLine1->stop();
    animationLine2->stop();
    animationLine3->stop();
    animationLine4->stop();
}

void btForm::pauseAnimation()
{
    // 调用 QPropertyAnimation 的 pause 方法
    animationLine1->pause();
    animationLine2->pause();
    animationLine3->pause();
    animationLine4->pause();
}

void btForm::resumeAnimation()
{
    // 调用 QPropertyAnimation 的 resume 方法
    animationLine1->start();
    animationLine2->start();
    animationLine3->start();
    animationLine4->start();
}

void btForm::mousePressEvent(QMouseEvent *event)
{
    (void)event;
    ui->btStyle->setStyleSheet("#btStyle{background-color: rgb(30, 206, 154);}"
                               "QLabel{background-color:transparent;}"
                               "#lineBox{background-color:transparent;}");

//    ui->btStyle->setStyleSheet("#btStyle{background-color: rgb(30, 206, 154);}");
//    ui->btIcon->setAttribute(Qt::WA_TranslucentBackground);
//    ui->btText->setAttribute(Qt::WA_TranslucentBackground);
    //发射信号
    emit btClick(this->pageId);
}
