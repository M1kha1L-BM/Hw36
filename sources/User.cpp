#include "User.h"
#include <iostream>

User::User(const std::string& login,
    const std::string& password,
    const std::string& salt,
    const std::string& name)
    : login(login), password(password), salt(salt), name(name) {
}

std::string User::getLogin() const { return login; }
std::string User::getPassword() const { return password; }
std::string User::getSalt() const { return salt; }
std::string User::getName() const { return name; }

void User::setConnected(bool value) { isConnected = value; }
void User::setBanned(bool value) { isBanned = value; }
bool User::getIsConnected() const { return isConnected; }
bool User::getIsBanned() const { return isBanned; }

void User::addMessage(const Message& msg) {
    inbox.push_back(msg);
}

void User::showInbox() const
{
    std::cout << "\n--- Сообщения для " << name << " ---\n";
    for (const auto& msg : inbox) {
        std::cout << msg.getSenderName() << " -> "
                  << msg.getDest() << ": " << msg.getText();
        if (msg.getIsPrivate())
            std::cout << " [приватное]";
        std::cout << std::endl;
    }
}
