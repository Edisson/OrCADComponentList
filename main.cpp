#include <QApplication>
//#include <QtWidgets/QApplication>
#include "view/mainwindow.h"
#include "CONST.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
