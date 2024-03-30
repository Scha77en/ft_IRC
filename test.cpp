#include <iostream>
#include <string>
#include <sstream>
#include "AR/Parse/Database.hpp"

void parseInput(const std::string& input, std::string& channel, std::string& topic) {
    // Trim leading and trailing spaces
    size_t startPos = input.find_first_not_of(" \t");
    size_t endPos = input.find_last_not_of(" \t");
    std::string trimmedInput = input.substr(startPos, endPos - startPos + 1);

    // Extract channel name
    size_t channelStart = trimmedInput.find('#');
    if (channelStart != std::string::npos) {
        size_t channelEnd = trimmedInput.find(' ', channelStart);
        if (channelEnd != std::string::npos) {
            channel = trimmedInput.substr(channelStart, channelEnd - channelStart);
        }
    }

    // Extract topic
    size_t topicStart = trimmedInput.find(':');
    if (topicStart != std::string::npos) {
        topic = trimmedInput.substr(topicStart + 1);
    }
}

int main() {
    std::string input = "TOPIC                #Channel           :     new topic";
//    std::string channel, topic;
    Database* db = Database::GetInstance();
    bool two_dots = TRUE;
//    parseInput(input, channel, topic);

//    std::cout << "Channel: " << channel << std::endl;
//    std::cout << "Topic: " << topic << std::endl;

	std::stringstream ss(input);
    std::string command;
    std::string channel;
    std::string topic;
    ss >> command >> channel;
    std::getline(ss, topic);

    topic = db->ExtractTopic(input, &two_dots);
    std::cout << "Command: " << command << std::endl;
    std::cout << "Channel: " << channel << std::endl;
    std::cout << "Topic: " << topic << std::endl;

    return 0;
}

