#include "kugoumusic.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KuGouMusic w;
    w.show();
    return a.exec();
}
