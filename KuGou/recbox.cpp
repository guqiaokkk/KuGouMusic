#include "recbox.h"
#include "ui_recbox.h"
#include "recboxitem.h"
#include <QJsonArray>
#include <QJsonObject>
#include <math.h>

RecBox::RecBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBox),
    row(1),
    col(4)
{
    ui->setupUi(this);

}

RecBox::~RecBox()
{
    delete ui;
}

void RecBox::initRecBoxUi(QJsonArray data, int row)
{
    // 如果是两⾏，说明当前RecBox是主界⾯上的supplyMusicBox
    if(row == 2)
    {
        this->row = row;
        this->col = 8;
    }
    else
    {
        // 否则：只有⼀⾏，为主界⾯上recMusicBox
        ui->recListDown->hide();
    }
    imageList = data;
    // 往RecBox中添加图⽚

    currentIndex = 0;
    count = ceil(imageList.size() / col);
    createRecBoxItem();
}

void RecBox::createRecBoxItem()
{
    // 创建RecBoxItem对象，往RecBox中添加
    // col

    // 溢出掉之前旧元素
    QList<RecBoxItem*> recUpList = ui->recListUp->findChildren<RecBoxItem*>();
    for(auto e : recUpList)
    {
        ui->recListUpHLayout->removeWidget(e);
        delete e;
    }
    QList<RecBoxItem*> recDownList = ui->recListDown->findChildren<RecBoxItem*>();
    for(auto e : recDownList)
    {
        ui->recListDownHLayout->removeWidget(e);
        delete e;
    }

    int index = 0;
    for(int i = currentIndex * col; i < currentIndex * col + col; ++i)
    {
        RecBoxItem *item = new RecBoxItem();

        QJsonObject obj = imageList[i].toObject();
        item->setText(obj.value("text").toString());
        item->setImage(obj.value("path").toString());

             // 将RecBoxItem对象添加到RecBox中
             // recMusicBox：col为4，元素添加到ui->recListUpHLayout中
             // supplyMuscBox: col为8， ui->recListUpHLayout添加4个，ui->recListDownHLayout添加4个
             // 即supplyMuscBox上下两⾏都要添加
             // 如果是recMusicBox：row为1，只能执⾏else，所有4个RecBoxItem都添加到ui->recListUpHLayout中
             // 如果是supplyMuscBox：row为2，col为8，col/2结果为4，i为0 1 2 3时，元素添加到ui->recListDownHLayout中
             // i为4 5 6 7时，元素添加到ui->recListUpHLayout中
        if(index >= col/2 && row == 2)
        {
             ui->recListDownHLayout->addWidget(item);
        }
        else
        {
            ui->recListUpHLayout->addWidget(item);
        }
        ++index;
    }
}

void RecBox::on_btUp_clicked()
{
    // 点击btUp按钮，显⽰前⼀组图⽚，如果已经是第⼀组图⽚，显⽰最后⼀组
    currentIndex--;
    if(currentIndex < 0)
    {
        currentIndex = count - 1;
    }
    createRecBoxItem();
}

void RecBox::on_btDown_clicked()
{
    // 点击btDown按钮，显⽰下⼀组图⽚，如果已经是最后⼀组图⽚，显⽰第0组
    currentIndex++;
    if(currentIndex >= count)
    {
        currentIndex = 0;
    }
    createRecBoxItem();
}
