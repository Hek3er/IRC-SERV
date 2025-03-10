#include "Server.hpp"

void    PrintLogo( void ) {
    const char* green = "\033[32m";
    const char* reset = "\033[0m";
    
    std::cout << green;
    std::cout << " ______  _______    ______          ______   ________  _______   __     __ " << std::endl;
    std::cout << "|      \\|       \\  /      \\        /      \\ |        \\|       \\ |  \\   |  \\" << std::endl;
    std::cout << " \\$$$$$$| $$$$$$$\\|  $$$$$$\\      |  $$$$$$\\| $$$$$$$$| $$$$$$$\\| $$   | $$" << std::endl;
    std::cout << "  | $$  | $$__| $$| $$   \\$$______|  $__\\$$| $$__    | $$__| $$| $$   | $$" << std::endl;
    std::cout << "  | $$  | $$    $$| $$     |      \\$$    \\ | $$  \\   | $$    $$ \\$$\\ /  $$" << std::endl;
    std::cout << "  | $$  | $$$$$$$\\| $$   __ \\$$$$$$_\\$$$$$$\\| $$$$$   | $$$$$$$\\  \\$$\\  $$ " << std::endl;
    std::cout << " _| $$_ | $$  | $$| $$__/  \\      |  \\__| $$| $$_____ | $$  | $$   \\$$ $$  " << std::endl;
    std::cout << "|   $$ \\| $$  | $$ \\$$    $$       \\$$    $$| $$     \\| $$  | $$    \\$$$   " << std::endl;
    std::cout << " \\$$$$$$ \\$$   \\$$  \\$$$$$$         \\$$$$$$  \\$$$$$$$$ \\$$   \\$$     \\$ " << std::endl << std::endl ;
    std::cout << reset;
}

bool CheckPort( const std::string _port ) {
    int len = _port.length();
    if (len != 4) {
        return false;
    }
    for (int i = 0; i < len; i++) {
        if (!std::isdigit(_port[i])) {
            return false;
        }
    }

    int p;
    std::stringstream ss;
    ss << _port;
    ss >> p;

    if (p < 1024) {
        return false;
    }
    return true;
}

void    checkPassword(std::string& password) {
    const std::string alphaNumeric = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";

    if (password.find_first_not_of(alphaNumeric) < (password.size() + 1)  ) {
        std::cout << password.size() << " " << password.find_first_not_of(alphaNumeric) << std::endl;
        std::cerr << "Password should contain only alpha numeric charachters " << std::endl;
        exit(1);
    }
}

int main( int ac, char **av ) {
    
    signal(SIGPIPE, SIG_IGN);
    if (ac != 3 || av[1][0] == '\0' || av[2][0] == '\0') {
        std::cerr << "Usage : ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    std::string port = std::string(av[1]);
    std::string password = std::string(av[2]);

    checkPassword(password);

    if (!CheckPort(port)) {
        std::cerr << "IRCSERV : [Port incorrect]" << std::endl;
        return 1;
    }

    PrintLogo();

	Server s(port, password);

	s.RunServer();

}
