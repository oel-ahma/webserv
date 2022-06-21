#include "../header/header.hpp" 
#include "../header/server.hpp" 

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
		return 1;
	}

	for (std::vector<ConfigParse>::iterator it = config._servers.begin(); it != config._servers.end(); it++) {
		Server	serv(*it);
		try {
			serv.set_server();
		}
		catch(const std::exception& e) {
			std::cerr << "Error occured: " << e.what() << std::endl;
			return (EXIT_FAILURE);
		}
		servers.push_back(serv);
	}
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
			usleep(300);
		}
	}
	return (0);
}
