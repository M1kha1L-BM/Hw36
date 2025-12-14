#include "Chat.h"
#include "Message.h"
#include "User.h"
#include "sha256.h"
#include <iostream>
#include <libpq-fe.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

// Генерация случайной соли
std::string generateSalt(size_t length = 16) {
    static const char chars[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(chars) - 2);

    std::string salt;
    for (size_t i = 0; i < length; ++i) {
        salt += chars[dist(gen)];
    }
    return salt;
}

Chat::Chat()
    : logger("log.txt"), network(logger) {
    conn = PQconnectdb("host=localhost dbname=postgres user=postgres password=12345678");
    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Ошибка подключения к БД: " << PQerrorMessage(conn) << endl;
        exit(1);
    }
    loadUsers();
    loadMessages();
}

Chat::~Chat() {
    PQfinish(conn);
    network.stop();
}

void Chat::saveMessage(const Message& msg) {
    if (!conn) return;

    const char* query =
        "INSERT INTO messages (sender, recipient, is_private, text) VALUES ($1, $2, $3, $4)";

    // Сохраняем строки в переменные
    std::string senderStr    = msg.getSenderLogin();
    std::string recipientStr = (msg.getDest() == -1 ? "all" : users[msg.getDest()]->getLogin());
    std::string isPrivateStr = msg.getIsPrivate() ? "true" : "false";
    std::string textStr      = msg.getText();

    const char* paramValues[4] = {
        senderStr.c_str(),
        recipientStr.c_str(),
        isPrivateStr.c_str(),
        textStr.c_str()
    };

    PGresult* res = PQexecParams(conn, query, 4, nullptr, paramValues, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Ошибка INSERT message: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);
}

std::shared_ptr<User> Chat::findUserByLogin(const std::string& login) const {
    for (const auto& user : users) {
        if (user->getLogin() == login) {
            return user;
        }
    }
    return nullptr;
}


void Chat::loadUsers() {
    const char* query = "SELECT login, password, salt, name FROM users";
    PGresult* res = PQexecParams(conn, query, 0, nullptr, nullptr, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Ошибка SELECT users: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        string login = PQgetvalue(res, i, 0);
        string password = PQgetvalue(res, i, 1);
        string salt = PQgetvalue(res, i, 2);
        string name = PQgetvalue(res, i, 3);
        users.push_back(make_shared<User>(login, password, salt, name));
    }
    PQclear(res);
}

void Chat::loadMessages() {
    const char* query =
        "SELECT m.sender, u.name, m.recipient, m.is_private, m.text "
        "FROM messages m "
        "JOIN users u ON m.sender = u.login "
        "ORDER BY m.id";

    PGresult* res = PQexecParams(conn, query, 0, nullptr, nullptr, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка SELECT messages: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        std::string senderLogin = PQgetvalue(res, i, 0);
        std::string senderName  = PQgetvalue(res, i, 1);
        std::string recipient   = PQgetvalue(res, i, 2);
        bool isPrivate          = (std::string(PQgetvalue(res, i, 3)) == "t");
        std::string text        = PQgetvalue(res, i, 4);

        int dest = -1;
        if (recipient != "all") {
            for (size_t j = 0; j < users.size(); ++j) {
                if (users[j]->getLogin() == recipient) {
                    dest = static_cast<int>(j);
                    break;
                }
            }
        }

        Message msg(senderLogin, dest, text, isPrivate);
        msg.setSenderName(senderName);
        allMessages.push_back(msg);

        if (recipient != "all") {
            auto user = findUserByLogin(recipient);
            if (user) user->addMessage(msg);
        } else {
            for (auto& user : users)
                if (user->getLogin() != senderLogin)
                    user->addMessage(msg);
        }
    }
    PQclear(res);
}

void Chat::registerUser() // consol only
{
    cout << "Регистрация нового пользователя" << endl;

    string login, password, name;

    cout << "Введите логин: ";
    cin >> login;
    if (findUserByLogin(login)) {
        cout << "Такой логин уже существует" << endl;

        return;
    }

    cout << "Введите пароль: ";
    cin >> password;
    string salt = generateSalt();
    string hashedPassword = sha256(password + salt);

    cout << "Введите имя: ";
    cin.ignore();
    getline(cin, name);

    const char* query =
        "INSERT INTO users (login, password, salt, name) VALUES ($1, $2, $3, $4)";
    const char* paramValues[4] = { login.c_str(), hashedPassword.c_str(), salt.c_str(), name.c_str() };

    PGresult* res = PQexecParams(conn, query, 4, nullptr, paramValues, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Ошибка INSERT user: " << PQerrorMessage(conn) << endl;
    }
    PQclear(res);

    users.push_back(make_shared<User>(login, hashedPassword, salt, name));
    cout << "Пользователь зарегистрирован" << endl;
}

int Chat::addUser(const std::string& login,    //only gui
                  const std::string& password,
                  const std::string& name) {
    // проверка уникальности
    if (findUserByLogin(login)) {
        return -2; // логин уже существует
    }

    std::string salt = generateSalt();
    std::string hashedPassword = sha256(password + salt);

    const char* query =
        "INSERT INTO users (login, password, salt, name) VALUES ($1, $2, $3, $4)";
    const char* paramValues[4] = {
        login.c_str(),
        hashedPassword.c_str(),
        salt.c_str(),
        name.c_str()
    };

    PGresult* res = PQexecParams(conn, query, 4, nullptr, paramValues, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Ошибка INSERT user: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return -1; // ошибка БД
    }
    PQclear(res);

    users.push_back(std::make_shared<User>(login, hashedPassword, salt, name.empty() ? login : name));
    return static_cast<int>(users.size() - 1); // возвращаем userId
}

int Chat::login(const std::string& login, const std::string& password)
{
    for (size_t i = 0; i < users.size(); ++i) {
        auto& user = users[i];
        if (user->getLogin() == login) {
            std::string hashedPassword = sha256(password + user->getSalt());
            if (user->getPassword() == hashedPassword) {
                loggedInUser = user;
                std::cout << "Добро пожаловать, " << user->getName() << "!" << std::endl;
                return static_cast<int>(i);
            } else {
                std::cout << "Неверный логин или пароль" << std::endl;
                return -1;
            }
        }
    }
    std::cout << "Неверный логин или пароль" << std::endl;
    return -1;
}

std::string Chat::getUserName(int userId) const {
    if (userId >= 0 && userId < static_cast<int>(users.size())) {
        return users[userId]->getName();
    }
    return {};
}

void Chat::logout() {
    if (loggedInUser) {
        cout << "Выход: " << loggedInUser->getName() << endl;
        loggedInUser = nullptr;
    }
    else {
        cout << "Вы не вошли в систему" << endl;
    }
}

void Chat::sendMessage() { // only cmd
    if (!loggedInUser) {
        std::cout << "Сначала войдите в систему" << std::endl;
        return;
    }

    std::string recipient, text;
    std::cout << "Кому (login или all): ";
    std::cin >> recipient;
    std::cout << "Текст: ";
    std::cin.ignore();
    std::getline(std::cin, text);

    bool isPrivate = (recipient != "all");

    // вычисляем ID получателя
    int dest = -1;
    if (recipient != "all") {
        for (size_t j = 0; j < users.size(); ++j) {
            if (users[j]->getLogin() == recipient) {
                dest = static_cast<int>(j);
                break;
            }
        }
    }

    Message msg(loggedInUser->getLogin(), dest, text, isPrivate);
    msg.setSenderName(loggedInUser->getName());

    const char* query =
        "INSERT INTO messages (sender, recipient, is_private, text) VALUES ($1, $2, $3, $4)";
    const char* paramValues[4] = {
        loggedInUser->getLogin().c_str(),
        recipient.c_str(),
        isPrivate ? "true" : "false",
        text.c_str()
    };

    PGresult* res = PQexecParams(conn, query, 4, nullptr, paramValues, nullptr, nullptr, 0);
    PQclear(res);

    allMessages.push_back(msg);

    if (recipient != "all") {
        auto user = findUserByLogin(recipient);
        if (user) user->addMessage(msg);
    } else {
        for (auto& user : users)
            if (user->getLogin() != loggedInUser->getLogin())
                user->addMessage(msg);
    }

    network.sendMessageToPeers(text);
    logger.writeLog("[Отправлено] " + text);
}

void Chat::addChatMessage(const std::string& sender,
                          const std::string& text) {
    Message msg(sender, -1, text, false);
    allMessages.push_back(msg);
    for (auto& user : users) {
        user->addMessage(msg);
    }
    saveMessage(msg);
}

void Chat::addPrivateMessage(const std::string& sender,
                             int dest,
                             const std::string& text) {
    if (dest < 0 || dest >= static_cast<int>(users.size())) {
        std::cerr << "Ошибка: некорректный ID получателя\n";
        return;
    }

    auto recipient = users[dest];
    if (recipient->getIsBanned()) {
        std::cerr << "Ошибка: пользователь " << recipient->getLogin()
            << " забанен, сообщение не отправлено\n";
        return;
    }

    Message msg(sender, dest, text, true);
    allMessages.push_back(msg);
    recipient->addMessage(msg);
    saveMessage(msg);
}

std::vector<std::string> Chat::getChatMessages() const {
    std::vector<std::string> result;
    for (const auto& msg : allMessages) {
        if (!msg.getIsPrivate()) {
            result.push_back(msg.getSenderLogin() + ": " + msg.getText());
        }
    }
    return result;
}

std::vector<Message> Chat::getPrivateMessage() const {
    std::vector<Message> result;
    for (const auto& msg : allMessages) {
        if (msg.getIsPrivate()) {
            result.push_back(msg);
        }
    }
    return result;
}

std::vector<std::string> Chat::getUserList() const {
    std::vector<std::string> result;
    for (const auto& user : users) {
        std::string status;
        if (user->getIsBanned()) {
            status = " [BANNED]";
        } else if (!user->getIsConnected()) {
            status = " [OFFLINE]";
        } else {
            status = " [ONLINE]";
        }
        result.push_back(user->getLogin() + status);
    }
    return result;
}

void Chat::checkMessages() {
    if (!loggedInUser) {
        cout << "Сначала войдите в систему" << endl;
        return;
    }
    loggedInUser->showInbox();
}

void Chat::disconnectUser(const std::string& login) {
    auto user = findUserByLogin(login);
    if (user) user->setConnected(false);
}

void Chat::banUser(const std::string& login) {
    auto user = findUserByLogin(login);
    if (user) user->setBanned(true);
}

void Chat::unbanUser(const std::string& login) {
    auto user = findUserByLogin(login);
    if (user) user->setBanned(false);
}

bool Chat::isUserBanned(int userId) const {
    if (userId < 0 || userId >= static_cast<int>(users.size())) return false;
    return users[userId]->getIsBanned();
}

void Chat::startNetworking() {
    unsigned short port;
    cout << "Введите порт сервера: ";
    cin >> port;
    network.startServer(port);

    string ip;
    cout << "Введите IP для подключения (или пусто): ";
    cin.ignore();
    getline(cin, ip);
    if (!ip.empty()) {
        unsigned short peerPort;
        cout << "Введите порт: ";
        cin >> peerPort;
        network.connectToPeer(ip, peerPort);
    }
}

void Chat::showLog() {
    logger.resetRead();
    std::string line;
    while (logger.readLog(line)) {
        std::cout << line << std::endl;
    }
}

std::vector<std::string> Chat::getAllMessages() const {
    std::vector<std::string> result;
    for (const auto& msg : allMessages) {
        std::string prefix = msg.getIsPrivate() ? "[PRIVATE] " : "[PUBLIC] ";
        result.push_back(prefix + msg.getSenderLogin() + ": " + msg.getText());
    }
    return result;
}

void Chat::run() {
    startNetworking();

    // while (true) {
    //     cout << "\n--- Меню ---\n";
    //     cout << "1. Регистрация\n";
    //     cout << "2. Вход\n";

    //     cout << "3. Отправить сообщение\n\r";

    //     cout << "4. Проверить сообщения\n\r";

    //     cout << "5. Выйти из аккаунта\n\r";

    //     cout << "6. Выход из программы\n\r";

    //     cout << "Выберите действие: ";

    //     string choice;
    //     cin >> choice;

    //     if (choice == "1") registerUser();
    //     else if (choice == "2") login();
    //     else if (choice == "3") sendMessage();
    //     else if (choice == "4") checkMessages();
    //     else if (choice == "5") logout();
    //     else if (choice == "6") {
    //         cout << "Сохранение данных и выход..." << endl;
    //         break;
    //     }
    //     else cout << "Неверный ввод" << endl;

    // }

}
