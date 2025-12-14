#include "contactswindow.h"
#include "ui_contactswindow.h"
#include <QMessageBox>

ContactsWindow::ContactsWindow(std::shared_ptr<Chat> dbPtr, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ContactsWindow),m_dbPtr(dbPtr)
{
    ui->setupUi(this);
    auto users = m_dbPtr->getUserList();
    for (const auto& u : users) {
        ui->usersList->addItem(QString::fromStdString(u));
    }

    connect(ui->disconnectButton, &QPushButton::clicked, this, &ContactsWindow::on_disconnectButton_clicked);
    connect(ui->banButton, &QPushButton::clicked, this, &ContactsWindow::on_banButton_clicked);
    connect(ui->unbanButton, &QPushButton::clicked, this, &ContactsWindow::on_unbanButton_clicked);
}

ContactsWindow::~ContactsWindow()
{
    delete ui;
}

void ContactsWindow::on_disconnectButton_clicked()
{
    auto* item = ui->usersList->currentItem();
    if (!item) return;

    std::string login = item->text().toStdString();
    login = login.substr(0, login.find(" "));
    m_dbPtr->disconnectUser(login);
    QMessageBox::information(this, "Disconnected",
                             QString("Пользователь %1 был отключён").arg(item->text()));
    refreshUsers();
}


void ContactsWindow::on_banButton_clicked()
{
    auto* item = ui->usersList->currentItem();
    if (!item) return;

    std::string login = item->text().toStdString();

    login = login.substr(0, login.find(" "));
    m_dbPtr->banUser(login);
    QMessageBox::warning(this, "Banned",
                         QString("Пользователь %1 был забанен").arg(item->text()));
    refreshUsers();
}


void ContactsWindow::on_unbanButton_clicked()
{
    auto* item = ui->usersList->currentItem();
    if (!item) return;

    std::string login = item->text().toStdString();
    login = login.substr(0, login.find(" "));
    m_dbPtr->unbanUser(login);
    QMessageBox::information(this, "Unbanned",
                             QString("Пользователь %1 был разбанен").arg(item->text()));
    refreshUsers();
}

void ContactsWindow::refreshUsers() {
    ui->usersList->clear();
    auto users = m_dbPtr->getUserList();
    for (const auto& u : users) {
        ui->usersList->addItem(QString::fromStdString(u));
    }
}

