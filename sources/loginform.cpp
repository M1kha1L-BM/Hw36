#include "loginform.h"
#include "ui_loginform.h"
#include <QMessageBox>

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginForm)
{
    ui->setupUi(this);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::setDatabase(std::shared_ptr<Chat> dbPtr)
{
    m_dbPtr = dbPtr;
}


void LoginForm::on_buttonBox_accepted()
{
    if (!m_dbPtr) {
        QMessageBox::critical(this, tr("Error"), tr("Database not available"));
        return;
    }

    // вызываем login() из Chat
    int userId = m_dbPtr->login(ui->loginLineEdit->text().toStdString(),
                                ui->passwordLineEdit->text().toStdString());

    if (userId < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Incorrect login or password"));
        return;
    }

    QString userName = QString::fromStdString(m_dbPtr->getUserName(userId));
    emit accepted(userId, userName);
}


void LoginForm::on_buttonBox_rejected()
{
    emit rejected();
}


void LoginForm::on_registrationPushButton_clicked()
{
    emit registerRequested();
}

