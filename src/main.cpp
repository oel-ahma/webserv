#include "../header/header.hpp" //TODO:
#include "../header/server.hpp" //TODO:
// #include "server.hpp"
// #include "header.hpp"

int main(int ac, char **av) 
{
	(void)ac;
	(void)av;
	// Config 						config;
	// std::vector<std::string>	configFile;
	// std::vector<Server>			Servers;

	// if (ac != 2)
  	// {
	// 	std::cerr << "Not Enough Argument!" << std::endl;
    // 	return (EXIT_SUCCESS);
  	// }
	// configFile = config.readConfigFile(av[1]);
	// try {
	// 	config.parse(configFile);
	// } catch (std::exception &e) {
	// 	std::cerr << e.what() << std::endl;
	// }
	// std::cout << config;
	//TODO: config à passer à la classe Server
	// iterer sur les conf ?
	// for (std::vector<std::string>::iterator it = configFile.begin(); it != configFile.end(); it++) {
		Server	serv;//TODO: not sur
		try {
			serv.set_server();
		}
		catch(const std::exception& e) {
			std::cerr << "Error occured: " << e.what() << std::endl;
			return (EXIT_FAILURE);
		}
		// Servers.push_back(serv);
	// std::vector<Server>::iterator it = Servers.begin();
	//le server tourne en boucle
	while (1) {
		// for (; it != Servers.end(); it++) {
			try {
				serv.routine();
			}
			catch(const std::exception& e) {
				std::cerr << "Error occured: " << e.what() << std::endl;
				return (EXIT_FAILURE);
			}
			usleep(300); //TODO: attention à la charge du cpu, utiliser un usleep() ou autre solution
		// }
		// it = Servers.begin();
	}
	return (0);
}
