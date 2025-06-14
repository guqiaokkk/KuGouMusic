#include "musicslider.h"
#include "ui_musicslider.h"

#include <QMouseEvent>

MusicSlider::MusicSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicSlider)
{
    ui->setupUi(this);

    // 初始情况下，还没有开始播放，将当前播放进度设置为0
    currentPos = 0;
    maxWidth = this->width();

    moveSilder();
}

MusicSlider::~MusicSlider()
{
    delete ui;
}

void MusicSlider::mousePressEvent(QMouseEvent *event)
{
    //QMouseEvent中的pos()为⿏标相对于widget的坐标，不是相当于screen
    currentPos = event->pos().x();
    moveSilder();
}

void MusicSlider::mouseReleaseEvent(QMouseEvent *event)
{
    currentPos = event->pos().x();
    moveSilder();

    emit setMusicSliderPosition((float)currentPos / (float)maxWidth);
}

void MusicSlider::mouseMoveEvent(QMouseEvent *event)
{
    // 如果⿏标不在MusicSlider的矩形内，不进⾏拖拽
    QRect rect = QRect(0, 0, geometry().width(), geometry().height());
    QPoint pos = event->pos();
    if(!rect.contains(pos))
    {
        return;
    }
    // 根据⿏标滑动的位置更新outLine的宽度
    if(event->buttons() == Qt::LeftButton)
    {
        // 验证：⿏标点击的x坐标是否越界，如果越界将其调整到边界
        currentPos = event->pos().x();
        if(currentPos < 0)
        {
            currentPos = 0;
        }
        if(currentPos > maxWidth)
        {
            currentPos = maxWidth;
        }
        moveSilder();
    }
}

void MusicSlider::moveSilder()
{
    // 根据当前进度设置外部滑动条的位置

    ui->outLine->setGeometry(ui->outLine->x(), ui->outLine->y(), currentPos, ui->outLine->height());//x 坐标：0 y 坐标：8 宽度：currentPos  高度：4
}
