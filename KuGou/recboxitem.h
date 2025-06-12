#ifndef RECBOXITEM_H
#define RECBOXITEM_H

#include <QWidget>

namespace Ui {
class RecBoxItem;
}

class RecBoxItem : public QWidget
{
    Q_OBJECT

public:
    explicit RecBoxItem(QWidget *parent = nullptr);
    ~RecBoxItem();

    bool eventFilter(QObject *watched, QEvent *event);

    void setText(const QString &text);
    void setImage(const QString &Imagepath);

private:
    Ui::RecBoxItem *ui;
};

#endif // RECBOXITEM_H
