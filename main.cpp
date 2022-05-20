#include "tools.hpp"
#include "config.hpp"

int main(int ac, char **av) 
{
	Config 						config;
	std::vector<std::string>	configFile;
  		
	if (ac != 2)
  	{
		std::cerr << "Not Enough Argument!" << std::endl;
    	return 1;
  	}
	configFile = config.readConfigFile(av[1]);
	try {
		config.parse(configFile);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	std::cout << config;
	/*
		while (true) { //le server tourne en boucle
			//fonction routine de Server.cpp
			//TODO: attention aux charge du cpu, utiliser un usleep() ou autre solution
		}
	*/
	return 0;
}
