#include "startscreen.h"
#include "ui_startscreen.h"

startscreen::startscreen(std::shared_ptr<Chat> dbPtr,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::startscreen)
{
    ui->setupUi(this);
    if(dbPtr)
        m_dbPtr = dbPtr;
    else
        m_dbPtr = std::make_shared<Chat>();

    ui->loginWidget->setDatabase(m_dbPtr);
    ui->registrationWidget->setDatabase(m_dbPtr);

    connect(ui->loginWidget,&LoginForm::registerRequested, this,&startscreen::setRegistrationForm);
    connect(ui->loginWidget,&LoginForm::accepted, this,&startscreen::onLoggedIn);
    connect(ui->loginWidget,&LoginForm::rejected, this,&startscreen::onRejectRequest);
    connect(ui->registrationWidget,&registrationform::loginRequested, this,&startscreen::setLoginForm);
    connect(ui->registrationWidget,&registrationform::accepted, this,&startscreen::onLoggedIn);
    connect(ui->registrationWidget,&registrationform::rejected, this,&startscreen::onRejectRequest);
}

startscreen::~startscreen()
{
    delete ui;
}

void startscreen::setLoginForm()
{
    ui ->stackedWidget->setCurrentIndex(0);
}

void startscreen::setRegistrationForm()
{
    ui ->stackedWidget->setCurrentIndex(1);
}

void startscreen::onLoggedIn(uint userId, QString userName)
{
    m_userId = userId;
    m_userName = userName;
    accept();

}

void startscreen::onRejectRequest()
{
    reject();
}

std::shared_ptr<Chat> startscreen::getDatabase() const
{
    return m_dbPtr;
}

QString startscreen::userName() const
{
    return m_userName;
}

int startscreen::userId() const
{
    return m_userId;
}
