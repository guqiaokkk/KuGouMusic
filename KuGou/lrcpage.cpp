#include "lrcpage.h"
#include "ui_lrcpage.h"

#include <QFile>
#include <QDebug>
#include <QPixmap>

// 引入头文件，用于计算文本像素宽度
#include <QFontMetrics>


LrcPage::LrcPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LrcPage)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);

    lrcAnimation = new QPropertyAnimation(this, "geometry", this);
    lrcAnimation->setDuration(500);
    lrcAnimation->setStartValue(QRect(15, 15, width(), height()));
    lrcAnimation->setEndValue(QRect(15, 15 + width(), width(), height()));

    // 点击设置下拉按钮时开启动画
    connect(ui->hideBtn, &QPushButton::clicked, this, [=]{
        lrcAnimation->start();
    });

    //动画结束时，将窗⼝隐藏
    connect(lrcAnimation, &QPropertyAnimation::finished, this, [=](){
        hide();
    });

    //ui->hideBtn->setIcon(QIcon(":/Image/.png"));


    // 设置所有歌词Label允许自动换行
    ui->line1->setWordWrap(true);
    ui->line2->setWordWrap(true);
    ui->line3->setWordWrap(true);
    ui->lineCenter->setWordWrap(true);
    ui->line4->setWordWrap(true);
    ui->line5->setWordWrap(true);
    ui->line6->setWordWrap(true);

}

LrcPage::~LrcPage()
{
    delete ui;
}

bool LrcPage::parseLrc(const QString &lrcPath)
{
    lrcLines.clear();

    // 打开歌词⽂件
    QFile lrcFile(lrcPath);
    if(!lrcFile.open(QFile::ReadOnly))
    {
        qDebug() << "打开文件失败:" << lrcPath;
        return false;
    }
    while(!lrcFile.atEnd())
    {
        //读行
        QString lrcWord = lrcFile.readLine(1024);

        // 解析此类型：[00:17.94]那些失眠的⼈啊 你们还好吗
        //           [0:58.600.00]你像⼀只⻜来⻜去的蝴蝶
        int left = lrcWord.indexOf('[');
        int right = lrcWord.indexOf(']');

        //解析时间
        qint64 lineTime = 0;
        int start = 0;
        int end = 0;
        QString time = lrcWord.mid(left, right - left + 1);

        //分
        start = 1;
        end = time.indexOf(':');
        lineTime += lrcWord.mid(start, end - start).toInt() * 60 * 1000;

        //秒
        start = end + 1;
        end = time.indexOf('.', start);
        lineTime += lrcWord.mid(start, end-start).toInt() * 1000;

        //毫秒
        start = end + 1;
        end = time.indexOf('.', start);
        lineTime += lrcWord.mid(start, end - start).toInt();

        //解析歌词
        QString word = lrcWord.mid(right + 1).trimmed();
        lrcLines.push_back(LrcLine(lineTime, word.trimmed()));
    }

    // 测试验证
    for(auto word : lrcLines)
    {
        qDebug()<<word.time<<" "<<word.text;
    }
    return true;
}

int LrcPage::getLineLrcWordIndex(qint64 pos)
{
    // 将time和QVector中保存的LrcLines中的time进行对比
    // 如果歌词是空的，返回-1
    if(lrcLines.isEmpty())
    {
        return -1;
    }

    if(lrcLines[0].time >= pos)
    {
        return 0;
    }

    // 通过时间⽐较，获取下标
    for(int i = 1; i < lrcLines.size(); ++i)
    {
        if(pos > lrcLines[i-1].time && pos <= lrcLines[i].time)
        {
            return i-1;
        }
    }

    // 如果没有找到，返回最后⼀⾏
    return lrcLines.size()-1;
}

QString LrcPage::getLineLrcWord(qint64 index)
{
    if(index < 0 || index >= lrcLines.size())
    {
        return "";
    }
    return  lrcLines[index].text;
}

void LrcPage::showLrcWord(qint64 nowtime)
{
   // 先要获取歌词--根据歌词的时间进⾏获取
    int index = getLineLrcWordIndex(nowtime); // 根据当前播放时间得到当前歌词句

    if(index == -1)
    {
        ui->line1->setText("");
        ui->line2->setText("");
        ui->line3->setText("");
        ui->lineCenter->setText("当前歌曲无歌词");
        ui->line4->setText("");
        ui->line5->setText("");
        ui->line6->setText("");
    }
    else
    {
        // 对每个 Label 的文本都进行包装处理
        ui->line1->setText(getWrappedText(ui->line1, getLineLrcWord(index - 3)));
        ui->line2->setText(getWrappedText(ui->line2, getLineLrcWord(index - 2)));
        ui->line3->setText(getWrappedText(ui->line3, getLineLrcWord(index - 1)));
        ui->lineCenter->setText(getWrappedText(ui->lineCenter, getLineLrcWord(index)));
        ui->line4->setText(getWrappedText(ui->line4, getLineLrcWord(index + 1)));
        ui->line5->setText(getWrappedText(ui->line5, getLineLrcWord(index + 2)));
        ui->line6->setText(getWrappedText(ui->line6, getLineLrcWord(index + 3)));
    }
}

void LrcPage::setAlbumCover(const QPixmap &cover)
{
    if (cover.isNull()) {
            // 如果传入的封面是空的，可以设置一个默认封面
            ui->imageBox->setPixmap(QPixmap(":/Image/KuGou.png")); // 这是默认图路径
        } else {
            ui->imageBox->setPixmap(cover);
        }
        // 确保图片能缩放以适应imageBox的大小
    ui->imageBox->setScaledContents(true);
}

QString LrcPage::getWrappedText(QLabel *label, const QString &text)
{
    if (text.isEmpty()) {
            return "";
        }

        // QFontMetrics 用于精确计算给定字体的文本宽度
        QFontMetrics metrics(label->font());
        int textWidth = metrics.horizontalAdvance(text);
        int labelWidth = label->width();

        // 如果文本宽度小于等于Label宽度，直接返回原文
        if (textWidth <= labelWidth) {
            return text;
        }

        // --- 如果文本过长，开始处理 ---
        // 目标：将文本分成两行，每行长度尽量接近
        int middle = text.length() / 2;
        int breakPoint = -1;

        // 从中间向两边寻找一个合适的分隔符（如空格、逗号、破折号）作为换行点
        for (int i = 0; i < middle; ++i) {
            if (text[middle - i].isSpace() || text[middle - i] == ',' || text[middle - i] == ' ' || text[middle-i] == '-') {
                breakPoint = middle - i;
                break;
            }
            if (text[middle + i].isSpace() || text[middle + i] == ',' || text[middle + i] == ' ' || text[middle+i] == '-') {
                breakPoint = middle + i;
                break;
            }
        }

        QString wrappedText;
        if (breakPoint != -1) {
            // 找到了合适的分隔符
            wrappedText = text.left(breakPoint) + "\n" + text.mid(breakPoint + 1);
        } else {
            // 如果没有找到分隔符，就在中间硬性换行
            wrappedText = text.left(middle) + "\n" + text.mid(middle);
        }

        return wrappedText;
}
