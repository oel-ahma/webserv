// #include "Response.hpp"
#include "../header/response.hpp" //TODO:

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Response::Response() : sizeBuff(0), responseIsSet(false), _contentTypeCgi(""), _responseLocation(NULL) {
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
	this->_addr = other._addr;
	this->_contentTypeCgi = other._contentTypeCgi;
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
/*                              MEMBER FUNCTION                               */
/* ************************************************************************** */

void	Response::prepareResponse(Request *request, ConfigParse const *config) {
	this->_request = request;
	this->_config = config;

	if (this->_request->getBody().size() > this->_config->getClientMaxBodySize())
		this->_request->setStatusCode(413);
	if (this->_request->getPath().size() > 1024)
		this->_request->setStatusCode(414);
	//Generate A Response From Request.
	this->createResponse();
	// std::cout << "My response buffer contains:\n" << this->buff << std::endl;
	this->responseIsSet = true;
}

void Response::createResponse() {
	std::map<std::string, ConfigParse> tmpConf(this->_config->getLocation());
	std::string tmpPath = this->_request->getPath();
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
	std::ostringstream tmpStatusCode;
	tmpStatusCode << this->_request->getStatusCode();
	this->_statusLine = this->_request->getVersion() + " " + tmpStatusCode.str() + " " + this->_statusMsg[this->_request->getStatusCode()] + "\r\n";
	this->setHeaders();
	//TODO: remplacer le body par le retour du CGI OU remplir le _body en conséquence
	this->_response = this->_statusLine + this->_headers + this->_body;
}


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
		if (this->_path.rfind(".") != std::string::npos && this->_path.substr(this->_path.rfind(".") + 1, std::string::npos) == this->_responseLocation->getCgiExtension())
		{
			execCGI();
			return 0;
		}
		file.open(this->_path.c_str(), std::fstream::in | std::fstream::out);
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
		std::fstream file(filePath.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
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
		if (!this->_contentTypeCgi.empty())
			this->_headers += "Content-Type: " + this->_contentTypeCgi + "\r\n";
	}
	std::ostringstream tmpBodyLength;
	tmpBodyLength << this->_body.length();
	this->_headers += "Content-Length: " + tmpBodyLength.str();
	this->_headers += "\r\n\r\n";
	return 0;
}

int 	Response::responseBodyErrorSet() {
	std::fstream errfile;
	std::string error_path;


	error_path = "." + this->_config->getRoot() + "/" + this->_config->getErrorPages()[this->_request->getStatusCode()];
	errfile.open(error_path.c_str(), std::fstream::in | std::fstream::out);
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
			while ((di_struct = readdir(dp)) != NULL)
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
						std::ostringstream tmpSize;
						tmpSize << static_cast<float>(file_stats.st_size) / 1000;
						std::string size = tmpSize.str();
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
		closedir(dp);
		this->_body += "</table></html>\r\n";
		removeDoubleSlash(this->_body);
		return 0;
}


void Response::execCGI()
{
    int input = dup(STDIN_FILENO);
    int output = dup(STDOUT_FILENO);

    FILE *fsInput = tmpfile();
    FILE *fsOutput = tmpfile();

    int fdInput = fileno(fsInput);
    int fdOutput = fileno(fsOutput);

    // std::cout << GREEN << "Body is: " << this->_request->getBody() << RESET << std::endl;
	write(fdInput, this->_request->getBody().c_str(), this->_request->getBody().size());
    lseek(fdInput, 0, SEEK_SET);

	pid_t pid = fork();
	if (pid == -1)
		this->_request->setStatusCode(502);	
	if (pid == 0)
	{
		extern char **environ;
		setEnvCGI();

		std::string tmpcgi = this->_responseLocation->getCgiPath();
		char const	*cgiInfo[3];
		cgiInfo[0] = tmpcgi.c_str();
		cgiInfo[1] = this->_path.c_str();
		cgiInfo[2] = NULL;
		
		dup2(fdInput, STDIN_FILENO);
		dup2(fdOutput, STDOUT_FILENO);
		if (execve(cgiInfo[0], (char *const *)cgiInfo, environ) == -1)
			exit(1);
	}
	int status;
	if (waitpid(pid, &status, 0) == -1)
		this->_request->setStatusCode(502);
	if (WIFEXITED(status) && WEXITSTATUS(status))
		this->_request->setStatusCode(502);
	lseek(fdOutput, 0, SEEK_SET);
	char buffer[BUFF];
	int rd;
	std::string tmp;
	while ((rd = read(fdOutput, buffer, BUFF - 1)) != 0) 
	{
		if (rd == -1) 
		{
			this->_request->setStatusCode(502);
			break;
		}
		tmp.append(buffer, rd);
	}
	dup2(STDIN_FILENO, input);
	dup2(STDOUT_FILENO, output);
	close(input);
	close(output);
	close(fdInput);
	close(fdOutput);
	fclose(fsInput);
	fclose(fsOutput);
	if (this->_request->getStatusCode() == 200)
	{
		size_t i = tmp.find("\r\n");
		if (i != std::string::npos)
		{
			std::string cgiStrHeaders = tmp.substr(0, i);
			cgiStrHeaders = cgiStrHeaders.substr(cgiStrHeaders.find_first_of(":") + 1, std::string::npos);
			this->_contentTypeCgi = cgiStrHeaders.substr(cgiStrHeaders.find_first_not_of(" "), std::string::npos);

			this->_body = tmp.substr(tmp.find("\r\n"), std::string::npos);
		}
		else
			this->_body = tmp;
	} 
}

void Response::setEnvCGI() {
	std::ostringstream tmpBodySize;
	tmpBodySize << this->_request->getBody().size();
	setenv("CONTENT_LENGTH", tmpBodySize.str().c_str(), 1);
	setenv("CONTENT_TYPE", "text/html", 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("PATH_INFO", this->_request->getPath().c_str(), 1);
	setenv("REQUEST_URI", this->_request->getPath().c_str(), 1);
	setenv("QUERY_STRING", this->_request->getQuery().c_str(), 1);
	setenv("REQUEST_METHOD", this->_request->getMethod().c_str(), 1);
	char buffer[16];
	inet_ntop(AF_INET, &_addr.sin_addr, buffer, sizeof(buffer));
	setenv("REMOTE_ADDR", buffer, 1);
	setenv("SCRIPT_NAME", this->_responseLocation->getCgiPath().c_str(), 1);
	setenv("SERVER_NAME", "webServ", 1);
	std::ostringstream tmpPort;
	tmpPort << this->_config->getListen().port;
	setenv("SERVER_PORT", tmpPort.str().c_str(), 1);
	setenv("SERVER_PROTOCOL", this->_request->getVersion().c_str(), 1);
	setenv("SERVER_SOFTWARE", "WebServ/42.42", 1);
	setenv("AUTH_TYPE", "", 1);
	setenv("PATH_TRANSLATED", this->_path.c_str(), 1);
	setenv("REMOTE_IDENT", "", 1);
	setenv("REMOTE_USER", "", 1);
	setenv("REDIRECT_STATUS", "200", 1);

	std::map<std::string, std::string> tmpHead = this->_request->getHeaders();
	for (std::map<std::string, std::string>::iterator it = tmpHead.begin(), \
		ite = tmpHead.end(); it != ite; it++)
	{
		std::string tmp = it->first;
		for (size_t i = 0; i < tmp.size(); i++)
			tmp[i] = toupper(tmp[i]);
		setenv(("HTTP_" + tmp).c_str(), it->second.c_str(), 1);
	}
}

void	Response::initStatusCodeMsg() {
	_statusMsg[200] = "OK";
	_statusMsg[400] = "BAD REQUEST";
	_statusMsg[403] = "FORBIDDEN";
	_statusMsg[404] = "NOT FOUND";
	_statusMsg[405] = "METHOD NOT ALLOWED";
	_statusMsg[413] = "PAYLOAD TOO LARGE";
	_statusMsg[414] = "URI TOO LONG";
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
