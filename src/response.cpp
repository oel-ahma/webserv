// #include "Response.hpp"
#include "../header/response.hpp" //TODO:

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Response::Response() : sizeBuff(0), responseIsSet(false), _responseLocation(NULL) {
	memset(buff, 0, BUFF);
	this->initStatusCodeMsg();
	this->initContentTypeMIME();
}

Response::Response(Response const &cpy) {
	*this = cpy;
}

/* ************************************************************************** */
/*                                 DESTRUCTOR                                 */
/* ************************************************************************** */

Response::~Response() {
}

/* ************************************************************************** */
/*                                 OPERATORS                                  */
/* ************************************************************************** */

Response &Response::operator=(Response const &other) {
	if (this == &other)
		return *this;
	this->sizeBuff = other.sizeBuff;
	this->responseIsSet = other.responseIsSet;
	this->_request = other._request;
	this->_config = other._config;
	this->_statusMsg = other._statusMsg;
	this->_statusLine = other._statusLine;
	this->_path = other._path;
	this->_headers = other._headers;
	this->_body = other._body;
	this->_response = other._response;
	this->_responseLocation = other._responseLocation;
	return *this;
}

void Response::clearAll() {
	this->sizeBuff = 0;
	this->responseIsSet = false;
	this->_request = NULL;
	this->_config = NULL;
	this->_responseLocation = NULL;
	this->_statusMsg.clear();
	this->_statusLine.clear();
	this->_path.clear();
	this->_headers.clear();
	this->_body.clear();
	this->_response.clear();
}

/* ************************************************************************** */
/*                                 ACCESSORS                                  */
/* ************************************************************************** */

std::string Response::getResponse() { return this->_response; }

/* ************************************************************************** */
/*                                  SETTERS	                                  */
/* ************************************************************************** */

/* ************************************************************************** */
/*                              MEMBER FUNCTION                               */
/* ************************************************************************** */

//TODO: mettre la fct dans classe CGI ?
void	execCGI() {
	//TODO:	Mettre le _body de la requete en INPUT pour le CGI, donc modifier le fd d'entrée standard
	if (dup2(/*int oldfd*/, STDIN_FILENO) == -1)
		// error

	//TODO: Rediriger l'OUTPUT vers une string/fichier pour recuperer la reponse du CGI
	if (dup2(/*int oldfd*/, STDOUT_FILENO) == -1)
		// error

	//TODO: Set les var d'env ?

	//TODO: se deplacer dans /usr/bin/ pour exec le php-cgi
	if (chdir("/usr/bin/") == -1)//path correct ?
		//error

	//TODO: exec le binaire
	if (execve(const char *fichier, char *const argv[], char *const envp[]) == -1)
		//error
}

void	Response::prepareResponse(Request *request, ConfigParse const *config) {
	this->_request = request;
	this->_config = config;

	if (this->_request->getBody().size() > this->_config->getClientMaxBodySize())
		this->_request->setStatusCode(413);
	// if (this->_request->getRequest() == "")
	// {
	// 	std::cout << YELLOW << "HERE" << std::endl;
	// 	this->_request->setStatusCode(400);

	// }
	//Generate A Response From Request.
	this->createResponse();
	// std::cout << "My response buffer contains:\n" << this->buff << std::endl;
	this->responseIsSet = true;
}

void Response::createResponse() {
	std::map<std::string, ConfigParse> tmpConf(this->_config->getLocation());
	std::string tmpPath = this->_request->getPath();
	std::cout << std::endl << GREEN << tmpPath << RESET << std::endl; //TODO:
	std::map<std::string, ConfigParse>::const_iterator it;

	// TMP = "/1.html"
	// TMP = "/inside/3.html"
	if (this->_request->getStatusCode() == 200)
	{
		while (1)
		{
			it = tmpConf.find(tmpPath);
			if (it != tmpConf.end())
			{
				this->_responseLocation = &(it->second);
				break ;
			}
			if (tmpPath == "/")
			{
				this->_request->setStatusCode(404);
				break ;
			}
			tmpPath = tmpPath.substr(0, tmpPath.rfind("/"));
			if (tmpPath.empty())
				tmpPath = "/";
		}
		this->_path =  "." + this->_config->getRoot() + this->_responseLocation->getRoot() + this->_request->getPath();
		if (this->_request->getPath() == "/")
			this->_path += this->_responseLocation->getIndex();
		/*else
			this->_path += ".html";*/
		std::vector<std::string> tmpAllwdMethod = _responseLocation->getAllowedMethods();
		for (std::vector<std::string>::const_iterator it = tmpAllwdMethod.begin(); it != tmpAllwdMethod.end(); it++)
		{
			if (*it == _request->getMethod())
				break;
			if (*it != _request->getMethod() && (it + 1) == tmpAllwdMethod.end())
				this->_request->setStatusCode(405);
		}
	}
	/*TODO: Où placer des fcts
	//TODO: reutilisation de tmpPath car plus utile. tmpPath sera égale à l'extension du fichier de la requete GET
	((tmpPath = this->_request->getPath()).erase(0, tmpPath.find_first_of(".") + 1)).resize(tmpPath.find_first_of(" "));
	if (tmpPath == "php")
		std::cout << std::endl << GREEN << "PHP file detected" << RESET << std::endl;
	pid_t pid = fork();
	if (pid == -1)
		// error
	else if (pid == 0)
		execCGI();
	else {
		if (wait(NULL) == -1)
			// error
	}*/
	if (this->_request->getStatusCode() == 200)
	{
		if (this->_request->getMethod() == "GET")//TODO: CGI ?
			this->httpGetMethod();
		if (this->_request->getMethod() == "POST")
			this->httpPostMethod();
		if (this->_request->getMethod() == "DELETE")
			this->httpDeleteMethod();
	}
	if (this->_request->getStatusCode() != 200)
		this->responseBodyErrorSet();
	this->_statusLine = this->_request->getVersion() + " " + std::to_string(this->_request->getStatusCode()) + " " + this->_statusMsg[this->_request->getStatusCode()] + "\r\n";
	this->setHeaders();
	//TODO: remplacer le body par le retour du CGI OU remplir le _body en conséquence
	this->_response = this->_statusLine + this->_headers + this->_body;
}

// void	Response::setResponseLocation() {
// }

int	Response::httpGetMethod() {
	std::fstream file;

	if (isDirectory(this->_path) && this->_responseLocation->getAutoindex() == true)
	{
		if (this->_responseLocation->getUploadEnable() == true)
			return responseBodyDirectorySet(1);
		return responseBodyDirectorySet(0);
	}
	if (isFile(this->_path))
	{
		file.open(this->_path, std::fstream::in | std::fstream::out);
		if (!file.fail())
		{
			std::string buff((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			this->_body = buff + "\r\n";
		}
		else
			this->_request->setStatusCode(403);
		file.close();
	}
	else
		this->_request->setStatusCode(404);
	return 0;
}

int Response::httpPostMethod() {


	std::string tmpContentType = this->_request->getHeaders().at("Content-Type");
	std::string boundary = "--" + tmpContentType.substr(tmpContentType.find("--"));
	
	tmpContentType = tmpContentType.substr(0, tmpContentType.find_first_of(";"));
	if (tmpContentType == "multipart/form-data")
	{
		std::string fileName;
		std::string filePath;
		std::string fileData;

		fileData = this->_request->getBody();
		fileName = fileData.substr(fileData.find("filename=\"") + 10, std::string::npos);
		fileName = fileName.substr(0, fileName.find_first_of("\""));
		filePath = "./" + this->_config->getRoot() + "/" + this->_responseLocation->getUploadPath() + "//" + fileName;
		fileData = fileData.substr(fileData.find("\r\n\r\n") + 4);
		fileData = fileData.substr(0, fileData.find(boundary));
		std::fstream file(filePath, std::fstream::in | std::fstream::out | std::fstream::trunc);
		if (!file.is_open())
		{
			this->_request->setStatusCode(403);
			file.close();
			return 1;
		}
		file << fileData;
		file.close();
		responseBodyDirectorySet(1);
	}
	else
	{
		this->_request->setStatusCode(400);
		return 1;
	}
	return 0;
}

int Response::httpDeleteMethod() {
	if (isDirectory(this->_path))
		this->_request->setStatusCode(403);
	if (access(this->_path.c_str(), W_OK) != 0)
	{
		this->_request->setStatusCode(403);
		return 1;
	}
	if (isFile(this->_path))
	{
		if (std::remove(this->_path.c_str()) != 0)
			this->_request->setStatusCode(500);
	}
	responseBodyDirectorySet(1);
	return 0;
}

int		Response::setHeaders() {
	//TODO: Which Headers to Put in...
	this->_headers += "Version: " + this->_request->getVersion() + "\r\n";
	this->_headers += "Connection: keep-alive\r\n";
	this->_headers += "Keep-Alive: timeout=10\r\n";
	if (isFile(this->_path))
	{
		size_t j = this->_path.rfind(".");
		if (j != std::string::npos)
		{
			std::string extension = this->_path.substr(j + 1 , std::string::npos);
			this->_headers += "Content-Type: " + _contentTypeMIME[extension] + "\r\n";
		}
	}
	this->_headers += "Content-Length: " + std::to_string(this->_body.length());
	this->_headers += "\r\n\r\n";

	return 0;
}

int 	Response::responseBodyErrorSet() {
	std::fstream errfile;
	std::string error_path;


	error_path = "." + this->_config->getRoot() + "/" + this->_config->getErrorPages()[this->_request->getStatusCode()];
	errfile.open(error_path, std::fstream::in | std::fstream::out);
	if (!errfile.fail())
	{
		std::string errbuff((std::istreambuf_iterator<char>(errfile)), std::istreambuf_iterator<char>());
		this->_request->setVersion("HTTP/1.1");
		this->_body = errbuff;
	}
	else
	{
		this->_request->setStatusCode(500);
		this->_request->setVersion("HTTP/1.1");
		this->_body = "<!doctype html><html><head><title>500 Internal Server Error</title><h1><b>Error 500</b></h1><h2>Internal Server Error</h2></head></html>\r\n";
	}
	errfile.close();
	return 0;
}

int		Response::responseBodyDirectorySet(size_t flag) {
		DIR				*dp;
		struct dirent	*di_struct;
		struct stat		file_stats;

		this->_body = "<html>";
		if (flag == 1)
			this->_body += "<head><script src=\"https://cdnjs.cloudflare.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script><script>function makeDELETErequest(url) {$.ajax({url: url,type: 'DELETE',});}</script></head>"; 
		this->_body += "<h1>Index of ";
		this->_body += this->_request->getPath() + "/</h1>";
		this->_body += "<table  style=\"width:100%; text-align:left; line-height: 2\">";
		this->_body += "<tr> <th>Name</th><th>Last modified</th><th>Size</th><th>Action</th> </tr>";
		if ((dp = opendir(this->_path.c_str())) != NULL)
		{
			while ((di_struct = readdir(dp)) != nullptr)
			{
				if (std::string(di_struct->d_name) != ".")
				{
					std::string tmp = this->_path + "/" + di_struct->d_name;
					stat(tmp.c_str(), &file_stats);
					this->_body += "<tr>";
					this->_body += "<td><a href = \"" + this->_request->getPath() + "/" + std::string(di_struct->d_name) + "\">" + di_struct->d_name;
					if (S_ISDIR(file_stats.st_mode))
						this->_body += "/";
					this->_body += "</a></td>";
					this->_body += "<td>";
					std::tm * ptm = std::localtime(&file_stats.st_mtime);
					char buffer[32];
					std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm); // Format: Mo, 15.06.2009 20:20:00
					this->_body += std::string(buffer);
					this->_body += "</td>";
					this->_body += "<td>";
					if (S_ISDIR(file_stats.st_mode))
						this->_body += "directory";
					else
					{
						std::string size = std::to_string(static_cast<float>(file_stats.st_size) / 1000);
						size = size.substr(0, size.size() - 3);
						this->_body +=  size + "Kb";
					}
					this->_body += "</td>";
					if (flag == 1 && !S_ISDIR(file_stats.st_mode))
					{
						this->_body += "<td><button style=\"cursor: pointer;\" onclick=\"makeDELETErequest('";
						this->_body += this->_request->getPath() + "/" + std::string(di_struct->d_name) + "')\">DELETE</button></td>";
					}
				}
				this->_body += "</tr>";
			} 
		}
		this->_body += "</table></html>\r\n";
		removeDoubleSlash(this->_body);
		return 0;
}

void	Response::initStatusCodeMsg() {
	_statusMsg[200] = "OK";
	_statusMsg[400] = "BAD REQUEST";
	_statusMsg[403] = "FORBIDDEN";
	_statusMsg[404] = "NOT FOUND";
	_statusMsg[405] = "METHOD NOT ALLOWED";
	_statusMsg[413] = "PAYLOAD TOO LARGE";
	_statusMsg[415] = "UNSUPPORTED MEDIA TYPE";
	_statusMsg[500] = "INTERNAL SERVER ERROR";
	_statusMsg[502] = "BAD GATEWAY";
}

void	Response::initContentTypeMIME() {
	std::vector<std::string> tmpContentType;
    size_t i(0);
    size_t j;
    std::ifstream file("mime.conf");

    if (!file.fail())
    {
        std::string buff((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        while (i != std::string::npos && j != std::string::npos)
        {
            i = buff.find_first_not_of(" \t\n");
            j = buff.find_first_of(" \t\n", i);
            if (j != std::string::npos)
            {
                tmpContentType.push_back(buff.substr(i, j - i));
                buff = buff.substr(j, std::string::npos);
            }
            else
                tmpContentType.push_back(buff.substr(i, std::string::npos - 1));
        }
		for (std::vector<std::string>::iterator it = tmpContentType.begin(); it != tmpContentType.end(); it++)
			_contentTypeMIME[*it] = *it++;
    }
	file.close();
}
