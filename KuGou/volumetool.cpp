#include "volumetool.h"
#include "ui_volumetool.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>

VolumeTool::VolumeTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeTool)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    setAttribute(Qt::WA_TranslucentBackground);

    //自定义阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor("#646464");
    shadowEffect->setBlurRadius(15);
    setGraphicsEffect(shadowEffect);

    // 给按钮设置图标
    ui->silenceBtn->setIcon(QIcon(":/Image/slicent.png"));
    //⾳量的默认⼤⼩20
    ui->outSlider->setGeometry(ui->outSlider->x(), 180 - 36 + 25, ui->outSlider->width(), 36);
    ui->sliderBtn->move(ui->sliderBtn->x(), ui->outSlider->y() - ui->sliderBtn->height()/2);
    ui->volumeRatio->setText("20%");
}

VolumeTool::~VolumeTool()
{
    delete ui;
}

void VolumeTool::paintEvent(QPaintEvent *event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);

    QPolygon polygon;
    polygon.append(QPoint(30,300));
    polygon.append(QPoint(70, 300));
    polygon.append(QPoint(50, 320));

    painter.drawPolygon(polygon);
}
