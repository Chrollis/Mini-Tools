#include "dialogformatter.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    DialogFormatter w;
    w.show();
    return a.exec();
}
