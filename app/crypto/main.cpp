#include <QApplication>
#include "Window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.setWindowTitle("Classic Cipher GUI — KTU");
    w.setWindowIcon(QIcon(":/images/ktu_logo.ico"));
    w.setMinimumSize(900, 700);
    w.show();
    return app.exec();
}
