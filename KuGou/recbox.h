#ifndef RECBOX_H
#define RECBOX_H

#include <QWidget>
#include <QJsonArray>

namespace Ui {
class RecBox;
}

class RecBox : public QWidget
{
    Q_OBJECT

public:
    explicit RecBox(QWidget *parent = nullptr);
    ~RecBox();

    void initRecBoxUi(QJsonArray data, int row);

    void createRecBoxItem();

private slots:
    void on_btUp_clicked();

    void on_btDown_clicked();

private:
    Ui::RecBox *ui;
    int row;
    int col;
    QJsonArray imageList;

    int currentIndex;
    int count;
};

#endif // RECBOX_H
