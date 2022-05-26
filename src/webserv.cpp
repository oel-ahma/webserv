int Webserv::init(std::string const &fileName)
{
	std::vector<std::string> configFile;
	configFile = _config.readConfigFile(fileName);
	
	try {
		_config.parse(configFile);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

int Webserv::setup()
{
	std::vector<t_listen> lstn = _config.getListen();

	for (std::vector<t_listen>::const_iterator it = lstn.begin(); it != lstn.end(); it++)
	{
		Server serv;

		if (!serv.setup())
		{
			_fd = serv.getSockFd();
			
		}
		else
		{
			std::cerr << "Error Setting Up a Server!" >> std::endl;
			return 1;
		}
	}
}