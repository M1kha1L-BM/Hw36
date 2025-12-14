#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QWidget>
#include <memory>
#include "Chat.h"


namespace Ui {
class registrationform;
}

class registrationform : public QWidget
{
    Q_OBJECT

public:
    explicit registrationform(QWidget *parent = nullptr);
    ~registrationform();
    void setDatabase(std::shared_ptr<Chat> dbPtr);

signals:
    void loginRequested();
    void accepted(int userId, QString userName);
    void rejected();

private slots:
    void on_loginpushButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::registrationform *ui;
    std::shared_ptr<Chat> m_dbPtr;
};

#endif // REGISTRATIONFORM_H
