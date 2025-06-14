#include "volumetool.h"
#include "ui_volumetool.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>

VolumeTool::VolumeTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeTool),
    isMuted(false),
    volumeRatio(20)
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
    ui->silenceBtn->setIcon(QIcon(":/Image/silence.png"));
    //⾳量的默认⼤⼩20
    ui->outSlider->setGeometry(ui->outSlider->x(), 180 - 36 + 25, ui->outSlider->width(), 36);
    ui->sliderBtn->move(ui->sliderBtn->x(), ui->outSlider->y() - ui->sliderBtn->height()/2);
    ui->volumeRatio->setText("20%");

    // 关联静⾳按钮的信号槽
    connect(ui->silenceBtn, &QPushButton::clicked, this, &VolumeTool::onSilenceBtnClicked);

    // 安装事件过滤器
    ui->volumeBox->installEventFilter(this);
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

void VolumeTool::onSilenceBtnClicked()
{
    isMuted = !isMuted;
    if(isMuted)
    {
        // true：表示静音
        ui->silenceBtn->setIcon(QIcon(":/Image/besilence.png"));
    }
    else
    {
        // false：表示非静音
        ui->silenceBtn->setIcon(QIcon(":/Image/silence.png"));
    }

    // 媒体静音设置，发射信号让KuGouMusic处理
    emit setSilence(isMuted);
}


bool VolumeTool::eventFilter(QObject *object, QEvent *event)
{
    // 过滤volumeBox上的事件
    if(object == ui->volumeBox)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            // 如果是⿏标按下事件，修改sliderBtn和outLine的位置，并计算volumeRation
            setVolume();
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            // 如果是⿏标释放事件，直接发射setMusicVolume信号
            emit setMusicVolume(volumeRatio);
        }
        else if(event->type() == QEvent::MouseMove)
        {
            // 如果是⿏标滚动事件，修改sliderBtn和outLine的位置，并计算volumeRation，
            setVolume();
            // 并发射setMusicVolume信号
            emit setMusicVolume(volumeRatio);
        }
        return true;
    }
    return QObject::eventFilter(object, event);
}

void VolumeTool::setVolume()
{
    // 1. 将⿏标的位置转换为sloderBox上的相对坐标，此处只要获取y坐标
    int height = ui->volumeBox->mapFromGlobal(QCursor().pos()).y();

    // 2. ⿏标在volumeBox中可移动的y范围在[25, 205之间]
    height = height < 25 ? 25 : height;
    height = height > 205? 205: height;

    // 3. 调整sliderBtn的位置
    ui->sliderBtn->move(ui->sliderBtn->x(), height - ui->sliderBtn->height()/2);

    // 4. 更新outSlider的位置和⼤⼩
    ui->outSlider->setGeometry(ui->outSlider->x(), height, ui->outSlider->width(), 205-height);

    // 5. 计算⾳量⽐率
    volumeRatio = (int)((int)ui->outSlider->height()/(float)180*100);

    // 6. 设置给label显⽰出来
    ui->volumeRatio->setText(QString::number(volumeRatio)+"%");
}
