#include "kugoumusic.h"

#include <QApplication>

#include <QMessageBox>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //创建共享内存
    QSharedMemory sharedMem("KuGou");

    // 当程序第一次运行时，由于共享内存的空间还没有创建，此处attch一定会失败，即返回false
    // 当程序第二次运行时，由于共享内存的空间已经被前一次的运行申请好，此时第二个实例再去关联就能成功
    // 由于只能运行一个实例，让第二个实例直接退出，即让第一个进程结束
    if(sharedMem.attach())
    {
        QMessageBox::information(nullptr, "KuGou", "KuGou运行中...");
        return 0;
    }

    sharedMem.create(1);

    KuGouMusic w;
    w.show();
    return a.exec();
}
