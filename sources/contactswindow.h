#ifndef CONTACTSWINDOW_H
#define CONTACTSWINDOW_H

#include <QDialog>
#include "Chat.h"
#include <memory>

namespace Ui {
class ContactsWindow;
}

class ContactsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ContactsWindow(std::shared_ptr<Chat> dbPtr = nullptr, QWidget *parent = nullptr);
    ~ContactsWindow();

private slots:

    void on_disconnectButton_clicked();

    void on_banButton_clicked();

    void on_unbanButton_clicked();
    void refreshUsers();

private:
    Ui::ContactsWindow *ui;
    std::shared_ptr<Chat> m_dbPtr;
};

#endif // CONTACTSWINDOW_H
