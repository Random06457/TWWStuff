#include <stdio.h>
#ifdef QT_BUILD
#include "Qt/MainWindow.h"
#include <QtWidgets/QApplication>
#else
#include "Cmd/CmdMain.hpp"
#endif

int main(int argc, char *argv[])
{
#ifdef QT_BUILD
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
#else
    ArgReader args(argc, const_cast<const char**>(argv));
    args.processHandlers(CmdMain::g_fileTypeHandler);
    
    return 0;
#endif
}
