#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

std::string getIPAddress() {
    char hostName[255];
    char ipAddress[255];
    
    // Get the hostname of the current machine
    if (gethostname(hostName, sizeof(hostName)) == -1) {
        perror("Error getting hostname");
        return "";
    }

    // Get host information using hostname
    struct hostent* hostInfo = gethostbyname(hostName);
    if (hostInfo == NULL) {
        perror("Error getting host information");
        return "";
    }

    // Convert the first IP address to a string
    if (inet_ntop(AF_INET, hostInfo->h_addr_list[0], ipAddress, sizeof(ipAddress)) == NULL) {
        perror("Error converting IP address to string");
        return "";
    }

    return std::string(ipAddress);
}

int main() {
    std::string ipAddress = getIPAddress();
    if (!ipAddress.empty()) {
        std::cout << "IP Address: " << ipAddress << std::endl;
    } else {
        std::cout << "Failed to retrieve IP address" << std::endl;
    }
    return 0;
}
