#include "historyformm.h"
#include "ui_historyformm.h"

historyformm::historyformm(std::shared_ptr<Chat> chatPtr, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::historyformm),
    m_dbPtr(chatPtr)
{
    ui->setupUi(this);

    updateUsers();
    updateMessages();
}

historyformm::~historyformm()
{
    delete ui;
}

void historyformm::updateUsers()
{
    if (!m_dbPtr) return;

    ui->usersListWidget->clear();
    auto users = m_dbPtr->getUserList();
    for (const auto& u : users) {
        ui->usersListWidget->addItem(QString::fromStdString(u));
    }
}

void historyformm::updateMessages()
{
    if (!m_dbPtr) return;

    ui->messagesBrowser->clear();
    auto allMessages = m_dbPtr->getAllMessages();
    for (const auto& msg : allMessages) {
        ui->messagesBrowser->append(QString::fromStdString(msg));
    }
}
