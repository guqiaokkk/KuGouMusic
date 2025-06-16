#include "musicslider.h"
#include "ui_musicslider.h"

#include <QMouseEvent>

MusicSlider::MusicSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicSlider)
{
    ui->setupUi(this);
    ui->outLine->raise();


    // 初始情况下，还没有开始播放，将当前播放进度设置为0
    currentPos = 0;   
    maxWidth = 0;
    // 初始时，让前景条宽度为0
    ui->outLine->setFixedWidth(0);
    updateSliderPosition();
}

MusicSlider::~MusicSlider()
{
    delete ui;
}

// 外部调用此函数来设置进度
void MusicSlider::setPosition(float percent)
{
    // 如果控件还没正确显示，maxWidth可能为0
    if (maxWidth <= 0) return;

    // 如果是100%，确保 currentPos 就是 width()
        if (qFuzzyCompare(percent, 1.0f)) {
            currentPos = this->width();
        } else {
            currentPos = static_cast<int>(qBound(0.0f, percent, 1.0f) * this->width());
        }
    updateSliderPosition();
}


void MusicSlider::mousePressEvent(QMouseEvent *event)
{
    emit sliderPressed();  //  发射按下信号

    //QMouseEvent中的pos()为⿏标相对于widget的坐标，不是相当于screen
    currentPos = event->pos().x();
    updateSliderPosition();

}

void MusicSlider::mouseReleaseEvent(QMouseEvent *event)
{
    (void)event;
    float percent = 0.0f;
    // 只有在释放鼠标时，才把最终的位置百分比发射出去
    if (maxWidth > 0) {
        // 我们不直接用 currentPos，而是再做一次校准
                int sliderHandleRadius = 5; // 假设滑块半径为5px

                // 如果用户拖得非常靠右（在终点附近的一个半径内），我们就强制认为是100%
                if (currentPos >= this->width() - sliderHandleRadius) {
                    percent = 1.0f;
                } else {
                    percent = (float)currentPos / (float)this->width();
                }
     }

     emit positionChanged(percent);
     emit sliderReleased(); // 通知主窗口：拖拽结束
}

void MusicSlider::mouseMoveEvent(QMouseEvent *event)
{
    // 如果⿏标不在MusicSlider的矩形内，不进⾏拖拽
    QRect rect = QRect(0, 0, width(), height());
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
        updateSliderPosition();
    }
}

// 控件大小改变
void MusicSlider::resizeEvent(QResizeEvent *event)
{
    // 实时更新控件的最大宽度
        maxWidth = event->size().width();
        QWidget::resizeEvent(event); // 调用父类的实现
}

void MusicSlider::updateSliderPosition()
{
    // 使用setGeometry来设置outLine的宽度
     ui->outLine->setFixedWidth(currentPos);
}
