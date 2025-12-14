#pragma once
#include <string>

class Message {
private:
    std::string senderLogin;
    std::string senderName;
    std::string text;
    bool isPrivate;
    int dest;

public:
    Message(const std::string& login,
           int dest,
           const std::string& text,
           bool isPrivate);

    std::string getSenderLogin() const;
    std::string getSenderName() const;
    std::string getText() const;
    bool getIsPrivate() const;
    int getDest() const;

    void setSenderName(const std::string& name);
}; 
