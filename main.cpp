#include "splicer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Splicer w;
    w.show();
    return a.exec();
}
