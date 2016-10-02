#include "mainwindow.h"
#include <QApplication>
#include "vypocetni.hpp"

int main(int argc, char *argv[])
{
    logfile.open("log.txt",ios_base::out);
    logfile << "--------------------------------------------------------------" << endl;
    logfile << __TIMESTAMP__ << endl;
    logfile.close();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    if(!logfile.is_open())
        logfile.open("log.txt", ios_base::app);
    logfile << "Program skončil běžným způsobem." << endl;
    logfile.flush();
    logfile.close();
    return a.exec();
}
