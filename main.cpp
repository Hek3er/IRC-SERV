#include "Server.hpp"

int main() {

	
	Client	c(1, "", "", "");

	std::string nick = "@hello";

	std::cout << c.CheckNickname(nick) << std::endl;


}