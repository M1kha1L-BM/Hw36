#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Chat.h"
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int userId,
                        QString userName,
                        std::shared_ptr<Chat> dbPtr = nullptr,
                        QWidget *parent = nullptr);
    ~MainWindow();
    static MainWindow* createClient(std::shared_ptr<Chat> dbPtr= nullptr);
    static MainWindow* ListOfContacts(std::shared_ptr<Chat> dbPtr= nullptr);
    static int kInstanceCount;

private slots:
    void on_sendMessageButton_clicked();

    void on_privateMessageSend_clicked();

    void on_messageLineEdit_returnPressed();

    void on_actionOpen_another_clent_triggered();

    void on_actionClose_all_triggered();

    void upDateChats();

    void on_actionList_of_contacts_triggered();

    void on_actionHistory_of_chat_triggered();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Chat> m_dbPtr;
    int m_userId;
    QString m_userName;
};

#endif // MAINWINDOW_H
