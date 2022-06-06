#include "../header/header.hpp" //TODO:
#include "../header/server.hpp" //TODO:
// #include "server.hpp"
// #include "header.hpp"

int main(int ac, char **av) 
{
	Config 						config;
	std::vector<std::string>	configFile;
	std::vector<Server>			servers;

	if (ac != 2)
  	{
		std::cerr << "Not Enough Argument!" << std::endl;
    	return (EXIT_SUCCESS);
  	}
	configFile = config.readConfigFile(av[1]);
	try {
		config.parse(configFile);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	// std::cout << config;
	//TODO: config à passer à la classe Server
	// iterer sur les conf ?
	for (std::vector<ConfigParse>::iterator it = config._servers.begin(); it != config._servers.end(); it++) {
		Server	serv(*it);//TODO: not sur
		try {
			serv.set_server();
		}
		catch(const std::exception& e) {
			std::cerr << "Error occured: " << e.what() << std::endl;
			return (EXIT_FAILURE);
		}
		servers.push_back(serv);
	}
	//le server tourne en boucle
	while (1) 
	{	
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) 
		{
			try {
				it->routine();
			}
			catch(const std::exception& e) {
				std::cerr << "Error occured: " << e.what() << std::endl;
				return (EXIT_FAILURE);
			}
			usleep(300); //TODO: attention à la charge du cpu, utiliser un usleep() ou autre solution
		}
		// it = servers.begin();
	}
	return (0);
}
