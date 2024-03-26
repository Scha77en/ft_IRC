#include "Client.hpp"

Client::Client() {
    
}
Client::~Client() {
    
}

Client::Client(int fd) {
	this->fd = fd;
}

Client::Client(const Client &client) {
	*this = client;
}

Client &Client::operator=(const Client &client) {
	this->fd = client.fd;
	this->_username = client._username;
	this->key = client.key;
	this->_nickname = client._nickname;
	this->_channels = client._channels;
	return *this;
}

void Client::setUsername(std::string username) {
	this->_username = username;
}

void Client::setRealname(std::string realname) {
	this->key = realname;
}

void Client::setNickname(std::string nickname) {
	this->_nickname = nickname;
}
