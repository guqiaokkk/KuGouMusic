#ifndef LISTITEMBOX_H
#define LISTITEMBOX_H

#include <QWidget>

namespace Ui {
class ListItemBox;
}

class ListItemBox : public QWidget
{
    Q_OBJECT

public:
    explicit ListItemBox(QWidget *parent = nullptr);
    ~ListItemBox();

    void setMusicName(const QString& name);
    void setSinger(const QString& singer);
    void setAlbumName(const QString& albumName);
    void setLikeIcon(bool like);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

signals:
    void setIsLike(bool);

private slots:

    void on_likeBtn_clicked();

private:
    Ui::ListItemBox *ui;
    bool isLike;
};

#endif // LISTITEMBOX_H
