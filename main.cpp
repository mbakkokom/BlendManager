#include "mainwindow.h"
#include <QApplication>

#include "main.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setArgument(argc, argv);

    w.bootstrap();
    //w.show();

    return a.exec();
}
