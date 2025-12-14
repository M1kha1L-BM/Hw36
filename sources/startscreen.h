#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QDialog>
#include <memory>
#include "Chat.h"

namespace Ui {
class startscreen;
}

class startscreen : public QDialog
{
    Q_OBJECT

public:
    explicit startscreen(std::shared_ptr<Chat> dbPtr = nullptr,QWidget *parent = nullptr);
    ~startscreen();
    void setLoginForm();
    void setRegistrationForm();

    int userId() const;
    QString userName() const;

    std::shared_ptr<Chat> getDatabase() const;

public slots:
    void onLoggedIn(uint userID, QString userName);
    void onRejectRequest();
private:
    Ui::startscreen *ui;
    int m_userId;
    QString m_userName;
    std::shared_ptr<Chat> m_dbPtr;
};

#endif // STARTSCREEN_H
