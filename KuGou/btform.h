#ifndef BTFORM_H
#define BTFORM_H

#include <QWidget>
#include <QPropertyAnimation>

namespace Ui {
class btForm;
}

class btForm : public QWidget
{
    Q_OBJECT

public:
    explicit btForm(QWidget *parent = nullptr);
    ~btForm();

    void setIconAndText(const QString &btIcon, const QString &btText, int pageId);

    void clearbg();

    int getId();

    void showAnimation();

    void hideAnimation(); // 隐藏动画的函数

    void pauseAnimation();   // 暂停动画的函数

    void resumeAnimation();  // 恢复动画的函数

protected:
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    void btClick(int fd);


private:
    Ui::btForm *ui;
    int pageId;

    QPropertyAnimation *animationLine1;
    QPropertyAnimation *animationLine2;
    QPropertyAnimation *animationLine3;
    QPropertyAnimation *animationLine4;
};

#endif // BTFORM_H
