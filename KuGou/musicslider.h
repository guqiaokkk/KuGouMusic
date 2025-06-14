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

    void moveSilder();

signals:
    void setMusicSliderPosition(float);

private:
    Ui::MusicSlider *ui;
    int currentPos;  // 滑动条当前位置
    int maxWidth;    // outLine的总宽度
};

#endif // MUSICSLIDER_H
