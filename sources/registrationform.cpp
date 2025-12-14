#include "registrationform.h"
#include <QMessageBox>
#include "ui_registrationform.h"

registrationform::registrationform(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::registrationform)
{
    ui->setupUi(this);
}

registrationform::~registrationform()
{
    delete ui;
}

void registrationform::setDatabase(std::shared_ptr<Chat> dbPtr)
{
    m_dbPtr = dbPtr;
}

void registrationform::on_loginpushButton_clicked()
{
    emit loginRequested();
}


void registrationform::on_buttonBox_accepted()
{
    if(ui->passwordLineEdit->text() !=
        ui->passwordConirmlineEdit->text())
    {
        QMessageBox::critical(this,tr("Error"),
                              tr("Password not match"));
        return;
    }
    if(ui->nameLineEdit->text() == "")
    {
        QMessageBox::critical(this,tr("Error"),
                              tr("Please, write your name"));
        return;
    }
    if(ui->loginLineEdit->text() == "")
    {
        QMessageBox::critical(this,tr("Error"),
                              tr("Login is null"));
        return;
    }

    auto userId = m_dbPtr -> addUser(ui->loginLineEdit->text().toStdString(),
                                   ui->passwordLineEdit->text().toStdString(),
                                   ui->nameLineEdit->text().toStdString());
    switch(userId)
    {
    case -1:
        QMessageBox::critical(this,tr("Error"),
                              tr("Incorrect login"));
        return;
    case -2:
        QMessageBox::critical(this,tr("Error"),
                              tr("Login alreadu exists"));
        return;
    default:
        emit accepted(userId, ui->loginLineEdit->text());
    }


}


void registrationform::on_buttonBox_rejected()
{
    emit rejected();
}

