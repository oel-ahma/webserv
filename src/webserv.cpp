// #include "../header/webserv.hpp"

// int Webserv::init(std::string const &fileName)
// {
// 	std::vector<std::string> configFile;
// 	configFile = this->_config.readConfigFile(fileName);
	
// 	try {
// 		this->_config.parse(configFile);
// 	} catch (std::exception &e) {
// 		std::cerr << e.what() << std::endl;
// 	}
// 	return 0;
// }

// int Webserv::setup()
// {
// 	std::vector<t_listen> lstn = this->_config.getListen();

// 	for (std::vector<t_listen>::const_iterator it = lstn.begin(); it != lstn.end(); it++)
// 	{
// 		Server serv;

// 		if (!serv.setup())
// 		{
// 			_fd = serv.getSockFd();
// 		}
// 		else
// 		{
// 			std::cerr << "Error Setting Up a Server!" >> std::endl;
// 			return 1;
// 		}
// 	}
// }