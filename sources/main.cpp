#include "Chat.h"
#include <iostream>
#include <QApplication>
#include "mainwindow.h"

using namespace std;

void printSystemInfo();

int main(int argc,char *argv[]) {

    QApplication a(argc,argv);
    auto w = MainWindow :: createClient();
    if(w)
        w -> show();
    else
        return 0;
    return a.exec();
    // setlocale(LC_ALL, "Russian");

    // printSystemInfo();

    Chat chat;
    chat.run();

    // return 0;
}
