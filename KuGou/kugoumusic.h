#ifndef KUGOUMUSIC_H
#define KUGOUMUSIC_H
#include "volumetool.h"
#include "musiclist.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class KuGouMusic; }
QT_END_NAMESPACE

class KuGouMusic : public QWidget
{
    Q_OBJECT

public:
    KuGouMusic(QWidget *parent = nullptr);
    ~KuGouMusic();

    void initUI();

    void connectSignalAndSlots();

    void onUpdateLikeMusic(bool isLike, QString musicId);



private slots:
    void on_quit_clicked();

    void onBtFromClick(int pageId);

    void on_volume_clicked();

    void on_addLocal_clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    QJsonArray RandomPicture();

private:
    Ui::KuGouMusic *ui;
    QPoint dragPosition;

    VolumeTool *volumeTool;

    MusicList musicList;

};
#endif // KUGOUMUSIC_H
