#include "Server.hpp"

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

    if (ac != 3) {
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

	Server s(port, password);

	s.RunServer();

}
