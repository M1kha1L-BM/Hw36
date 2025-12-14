#ifndef HISTORYFORMM_H
#define HISTORYFORMM_H

#include <QDialog>
#include "Chat.h"
#include <memory>

namespace Ui {
class historyformm;
}

class historyformm : public QDialog
{
    Q_OBJECT

public:
    explicit historyformm(std::shared_ptr<Chat> chatPtr, QWidget *parent = nullptr);
    ~historyformm();

private slots:
    void updateUsers();
    void updateMessages();

private:
    Ui::historyformm *ui;
    std::shared_ptr<Chat> m_dbPtr;
};

#endif // HISTORYFORMM_H
