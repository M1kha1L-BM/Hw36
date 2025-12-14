#pragma once
#include <memory>
#include <vector>
#include <string>
#include "User.h"
#include "Message.h"
#include "Network.h"
#include "Logger.h"
#include <libpq-fe.h>

class Chat {
private:
    std::vector<std::shared_ptr<User>> users;
    std::vector<Message> allMessages;
    std::shared_ptr<User> loggedInUser = nullptr;
    Logger logger; // כמדדונ
    Network network;
    PGconn* conn;

    void loadUsers();
    void loadMessages();
    std::shared_ptr<User> findUserByLogin(const std::string& login)  const;
    void saveMessage(const Message& msg);

public:
    Chat();
    ~Chat();

    void registerUser();
    int addUser(const std::string& login,
                const std::string& password,
                const std::string& name );
    int login(const std::string& login, const std::string& password);
    std::string getUserName(int userId) const;
    void logout();
    void sendMessage();
    void addChatMessage(const std::string& sender, const std::string& text);
    void addPrivateMessage(const std::string& sender,
                           int dest,
                           const std::string& text);

    std::vector<std::string> getChatMessages() const;
    std::vector<Message> getPrivateMessage() const;
    std::vector<std::string> getUserList() const;
    std::vector<std::string> getAllMessages() const;
    void disconnectUser(const std::string& login);
    void banUser(const std::string& login);
    void unbanUser(const std::string& login);
    void checkMessages();
    void startNetworking();
    void run();
    void showLog();
    bool isUserBanned(int userId) const;
};
