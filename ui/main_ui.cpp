#include <QApplication>
#include <QIcon>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("CodeCoach++");
    app.setOrganizationName("CodeCoach");
#ifdef HAVE_QT_SVG
    app.setWindowIcon(QIcon(":/images/icon.svg"));
#endif

    MainWindow win;
    win.show();

    return app.exec();
}
