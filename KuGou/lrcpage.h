#ifndef LRCPAGE_H
#define LRCPAGE_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QLabel>

// LRC行歌词结构
struct LrcLine
{
    qint64 time;
    QString text;
    LrcLine(qint64 qtime, QString qtext)
             :time(qtime)
             ,text(qtext)
    {}
};


namespace Ui {
class LrcPage;
}

class LrcPage : public QWidget
{
    Q_OBJECT

public:
    explicit LrcPage(QWidget *parent = nullptr);
    ~LrcPage();

    bool parseLrc(const QString &lrcPath);

    int getLineLrcWordIndex(qint64);

    QString getLineLrcWord(qint64 index);

    void showLrcWord(qint64 nowtime);

     void setAlbumCover(const QPixmap& cover);//设置图片


private:
      QString getWrappedText(QLabel* label, const QString& text);

private:
    Ui::LrcPage *ui;

    QPropertyAnimation *lrcAnimation;

    QVector<LrcLine> lrcLines;// 按照时间的先后次序保存每⾏歌词
};

#endif // LRCPAGE_H
