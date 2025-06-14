#ifndef VOLUMETOOL_H
#define VOLUMETOOL_H

#include <QWidget>

namespace Ui {
class VolumeTool;
}

class VolumeTool : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeTool(QWidget *parent = nullptr);
    ~VolumeTool();

    void paintEvent(QPaintEvent *event);

    void onSilenceBtnClicked(); //静音槽函数

    bool eventFilter(QObject* object, QEvent* event);// 事件过滤器

    void setVolume();   // 根据⿏标在滑竿上滑动更新滑动界⾯，并按照⽐例计算⾳量⼤⼩

signals:
    void setSilence(bool); //设置静音

    void setMusicVolume(int volume);//修改⾳量⼤⼩


private:
    Ui::VolumeTool *ui;

    bool isMuted;    // 记录静⾳或⾮静⾳，当点击静⾳按钮时，在true和false之间切换
    int volumeRatio; // 标记⾳量⼤⼩
};

#endif // VOLUMETOOL_H
