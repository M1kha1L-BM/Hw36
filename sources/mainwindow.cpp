#include "mainwindow.h"
#include "startscreen.h"
#include "contactswindow.h"
#include "ui_mainwindow.h"
#include "historyformm.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTimer>
#include <QMessageBox>

int MainWindow::kInstanceCount = 0;

MainWindow::MainWindow(int userId,QString userName,std::shared_ptr<Chat> dbPtr, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_userId(userId),
    m_userName(userName)
{
    ui->setupUi(this);
    kInstanceCount++;
    if(dbPtr)
        m_dbPtr = dbPtr;
    else
        m_dbPtr = std::make_shared<Chat>();
    auto timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this, &MainWindow::upDateChats);
    timer->start(50);
}

MainWindow::~MainWindow()
{
    delete ui;
    kInstanceCount--;
    if(kInstanceCount <= 0)
        qApp->exit(0);
}

MainWindow *MainWindow::createClient(std::shared_ptr<Chat> dbPtr)
{
    startscreen s(dbPtr);
    auto result = s.exec();
    if(result == QDialog::Rejected)
    {
        return nullptr;
    }
    auto w = new MainWindow(s.userId(),s.userName(), s.getDatabase());
    w ->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}


void MainWindow::on_sendMessageButton_clicked()
{
    m_dbPtr->addChatMessage(m_userName.toStdString(),
                            ui->messageLineEdit->text().toStdString());
}


void MainWindow::on_privateMessageSend_clicked()
{
    QDialog dial(this);
    dial.setModal(true);
    auto l =  new QVBoxLayout();
    dial.setLayout(l);
    auto userListWgt = new QListWidget(&dial);
    l ->addWidget(userListWgt);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel,
                                          &dial);
    l -> addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted,&dial,&QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected,&dial,&QDialog::reject);

    auto userList = m_dbPtr->getUserList();
    for(auto user: userList)
    {
        userListWgt->addItem(QString::fromStdString(user));
    }

    userListWgt->setCurrentRow(0);

    auto result = dial.exec();

    if(result == QDialog::Accepted && userListWgt->currentItem())
    {
        std::string recipientLogin = userListWgt->currentItem()->text().toStdString();
        int dest = -1;
        for (size_t i = 0; i < m_dbPtr->getUserList().size(); ++i) {
            if (m_dbPtr->getUserList()[i] == recipientLogin) {
                dest = static_cast<int>(i);
                break;
            }
        }

        if (m_dbPtr->isUserBanned(dest)) {
            QMessageBox::warning(this,
                                 tr("Отправка невозможна"),
                                 tr("Пользователь %1 забанен, сообщение не отправлено.")
                                     .arg(QString::fromStdString(recipientLogin)));
            return;
        }

        m_dbPtr->addPrivateMessage(m_userName.toStdString(),
                                   dest,
                                   ui->messageLineEdit->text().toStdString());
    }
}


void MainWindow::on_messageLineEdit_returnPressed()
{
    m_dbPtr->addChatMessage(m_userName.toStdString(),
                            ui->messageLineEdit->text().toStdString());
}


void MainWindow::on_actionOpen_another_clent_triggered()
{
    auto w = createClient(m_dbPtr);
    if(w)
        w->show();
}


void MainWindow::on_actionClose_all_triggered()
{
    this ->close();
}

void MainWindow::upDateChats()
{
    auto chatMessages  =  m_dbPtr ->getChatMessages();
    QString chat;
    for(const auto &msg:chatMessages)
    {
        chat.append(QString::fromStdString(msg)+"\n");
    }
    if(ui->commonChatBrowser->toPlainText() != chat)
    {
        ui->commonChatBrowser->setText(chat);
    }
    chat.clear();
    auto privateMessages = m_dbPtr->getPrivateMessage();
    for(const auto &msg : privateMessages)
    {
        if(QString:: fromStdString( msg.getSenderName()) != m_userName &&
            msg.getDest() != m_userId)
        {
            continue;
        }

        QString prefix;
        if(m_userName == QString::fromStdString(msg.getSenderName())&&
            m_userId == msg.getDest())
        {
            prefix = tr("self message")+ ": ";
        }
        else if(m_userName == QString::fromStdString(msg.getSenderName()))
        {
            prefix = tr("message to") +
                     QString("<%1: ").
                     arg(QString::fromStdString(m_dbPtr->getUserName(msg.getDest())));
        }
        else
        {
            prefix = "<" + QString::fromStdString(msg.getSenderName())+
                     ">:" + tr("say to you")+": ";
        }
        chat.append(prefix +QString::fromStdString(msg.getText())+ "\n");
    }
    if(ui->privateChatBrowser->toPlainText() != chat)
    {
        ui->privateChatBrowser->setText(chat);
    }
}


void MainWindow::on_actionList_of_contacts_triggered()
{
    ContactsWindow* contactsWin = new ContactsWindow(m_dbPtr, this);
    contactsWin->setAttribute(Qt::WA_DeleteOnClose);
    contactsWin->show();
}


void MainWindow::on_actionHistory_of_chat_triggered()
{
    auto historyWin = new historyformm(m_dbPtr, this);
    historyWin->setAttribute(Qt::WA_DeleteOnClose);
    historyWin->show();

}

