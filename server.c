#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h> // Include the necessary header for inet_pton
#include <time.h>

// Function to generate a session ID
char *generateSessionID() {
    time_t t;
    time(&t);
    struct tm *tm_info = localtime(&t);
    char *sessionID = (char *)malloc(20);  // Allocate memory for the session ID
    strftime(sessionID, 20, "%Y%m%d%H%M%S", tm_info);
    return sessionID;
}

int main() {
	while(1){
    // Generate a session ID
    char *sessionID = generateSessionID();

    // Define the target URL and port
    const char *url = "192.168.111.153"; // Firewall IP
    const char *path = "/api/activate-firewall"; // API endpoint to activate the firewall
    const int port = 80; // You may need to adjust the port

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Error opening socket");
        free(sessionID);
        exit(1);
    }

    // Initialize the server address struct
    struct sockaddr_in serverAddr;
    bzero((char *)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    // Use inet_pton to convert the IP address to binary form
    if (inet_pton(AF_INET, url, &(serverAddr.sin_addr)) <= 0) {
        perror("Invalid IP address");
        free(sessionID);
        close(clientSocket);
        exit(1);
    }

    // Connect to the server
    while (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error connecting to the server");
        free(sessionID);
        close(clientSocket);
        //exit(1);
    }

    // Construct the HTTP POST request to activate the firewall
    char request[256];
    snprintf(request, sizeof(request), "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Length: 0\r\n\r\n", path, url);

    // Send the request to the server
    int n = write(clientSocket, request, strlen(request));
    if (n < 0) {
        perror("Error writing to socket");
        free(sessionID);
        close(clientSocket);
        exit(1);
    }

    // Log the date, time, and session ID for the request
    time_t currentTime;
    time(&currentTime);
    struct tm *tm_info = localtime(&currentTime);
    char timeStr[20];
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s][Session: %s] Request sent to %s:%d\n", timeStr, sessionID, url, port);

    // Wait for the firewall activation response (in a real scenario, handle the API response)

    // Log the date, time, and session ID for the response (simulated success)
    time(&currentTime);
    tm_info = localtime(&currentTime);
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s][Session: %s] Firewall activation successful\n", timeStr, sessionID);

    // Clean up and close the socket
    close(clientSocket);
    free(sessionID);

    return 0;
	
	}
}
