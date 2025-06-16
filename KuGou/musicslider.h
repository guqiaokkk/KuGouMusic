#ifndef MUSICSLIDER_H
#define MUSICSLIDER_H

#include <QWidget>


namespace Ui {
class MusicSlider;
}

class MusicSlider : public QWidget
{
    Q_OBJECT

public:
    explicit MusicSlider(QWidget *parent = nullptr);
    ~MusicSlider();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);  //响应窗口大小改变

private:
    // 私有辅助函数，用于根据 currentPos 更新UI
    void updateSliderPosition();



signals:
    // 信号，在用户操作完成后发射，通知外部新的进度百分比
    void positionChanged(float percent);
    // 报告拖拽状态,开始和结束
    void sliderPressed();
    void sliderReleased();

public slots:
    // 接收外部百分比，更新UI
    void setPosition(float percent);


private:
    Ui::MusicSlider *ui;
    int currentPos;  // 滑动条当前位置
    int maxWidth;    // outLine的总宽度


};

#endif // MUSICSLIDER_H
