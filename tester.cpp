#include <iostream>
#include <vector>
#include <csignal>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>

// Global vector to store child process IDs
std::vector<pid_t> childPIDs;

// Signal handler for `Ctrl+C` (SIGINT)
void handleSigint(int /*signum*/) {
    std::cout << "\nTerminating all nc instances..." << std::endl;

    // Terminate all child processes
    for (pid_t pid : childPIDs) {
        kill(pid, SIGTERM); // Send SIGTERM to the child process
    }

    // Clear the list of child processes
    childPIDs.clear();

    // Exit the parent process
    exit(0);
}

// Function to run `nc` in a child process
void runNc(const std::string& address, int port) {
    pid_t pid = fork(); // Create a new process
    if (pid == -1) {
        std::cerr << "Failed to fork a process" << std::endl;
        return;
    }

    if (pid == 0) {
        // Child process: Execute the `nc` command
        std::string portStr = std::to_string(port);
        execlp("nc", "nc", address.c_str(), portStr.c_str(), nullptr);

        // If execlp returns, it failed
        std::cerr << "Failed to execute nc command" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // Parent process: Add the child PID to the list
        childPIDs.push_back(pid);
    }
}

int main() {
    std::string address = "127.0.0.1";  // Default address
    int port = 6667;                   // Default port
    int numInstances;                  // Number of instances to run

    // std::cout << "Enter the number of nc instances to run: ";
    // std::cin >> numInstances;
    numInstances = 100;
    if (numInstances <= 0) {
        std::cerr << "Number of instances must be greater than 0." << std::endl;
        return 1;
    }

    // Set up the signal handler for `Ctrl+C`
    signal(SIGINT, handleSigint);

    // Launch the specified number of nc instances
    for (int i = 0; i < numInstances; ++i) {
        runNc(address, port);
    }

    std::cout << numInstances << " instances of nc are running in parallel. Press Ctrl+C to terminate them." << std::endl;

    // Keep the parent process running
    while (true) {
        pause(); // Wait for a signal
    }

    return 0;
}
