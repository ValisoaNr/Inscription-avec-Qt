#include "Inscription.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Inscription w;
    w.show();
    return QCoreApplication::exec();
}
