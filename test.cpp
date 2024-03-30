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

void processModeArguments(std::vector<std::string>& arguments) {
    if (!arguments.empty()) {
        std::cout << "Processing mode argument: " << arguments.front() << std::endl;
        // Remove the processed argument from the vector
        arguments.erase(arguments.begin());
    } else {
        std::cout << "No more mode arguments to process." << std::endl;
    }
}

int main() {
    std::string input = "TOPIC                #Channel           55     aa ff gg tt";
//    std::string channel, topic;
//    parseInput(input, channel, topic);

//    std::cout << "Channel: " << channel << std::endl;
//    std::cout << "Topic: " << topic << std::endl;

	std::stringstream ss(input);
    std::string command, channel;
    ss >> command >> channel;

    std::vector<std::string> args;

    for (std::string arg; ss >> arg;) {
        args.push_back(arg);
    }

    std::cout << "Command: " << command << std::endl;
    std::cout << "Channel: " << channel << std::endl;
    
    for (const auto& arg : args) {
        std::cout << arg << std::endl;
    }

    processModeArguments(args);

    std::cout << "********************************" << std::endl;

    for (const auto& arg : args) {
        std::cout << arg << std::endl;
    }


    return 0;
}

