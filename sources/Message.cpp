#include "Message.h"

Message::Message(const std::string& login,
                 int dest,
                 const std::string& text,
                 bool isPrivate)
    : senderLogin(login), text(text), isPrivate(isPrivate), dest(dest) {}

std::string Message::getSenderLogin() const { return senderLogin; }
std::string Message::getSenderName() const { return senderName; }
int Message::getDest() const { return dest; }
std::string Message::getText() const { return text; }
bool Message::getIsPrivate() const { return isPrivate; }

void Message::setSenderName(const std::string& name) { senderName = name; }
