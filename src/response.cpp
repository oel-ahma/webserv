// #include "Response.hpp"
#include "../header/response.hpp" //TODO:

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Response::Response() : sizeBuff(0), responseIsSet(false), file_fd(-1), _responseLocation(NULL) {
	memset(buff, 0, BUFF);
	this->initStatusCodeMsg();
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
	this->file_name = other.file_name;
	this->file_fd = other.file_fd;
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

/* ************************************************************************** */
/*                                 ACCESSORS                                  */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                  SETTERS	                                  */
/* ************************************************************************** */

/* ************************************************************************** */
/*                              MEMBER FUNCTION                               */
/* ************************************************************************** */


void	Response::prepareResponse(Request *request, ConfigParse const *config) {
	this->_request = request;
	this->_config = config;

	if (this->_request->getBody().size() > this->_config->getClientMaxBodySize())
		this->_request->setStatusCode(413);
	if (this->file_stream.is_open() == false) {
		// std::cout << "creation du fichier pour l'envoi." << std::endl;
		this->CreateTmpFile();
	}
	//Generate A Response From Request.
	this->createResponse();
	//Put The Response In the File.
	this->file_stream << this->_response;
	//Fill The Buffer From the File.
	this->file_stream.close();
	this->file_stream.open(this->file_name.c_str(), std::fstream::in | std::fstream::out);
	this->file_stream.read(this->buff, BUFF);
	if (this->file_stream.bad())
			throw std::ios_base::failure("failed to read file: " + this->file_name);
	this->sizeBuff = this->file_stream.gcount();
	// std::cout << "My response buffer contains:\n" << this->buff << std::endl;
	// std::cout << "sizeBuff = " << this->sizeBuff << std::endl;
	this->responseIsSet = true;
}

void	Response::CreateTmpFile() {
	this->file_name = "/tmp/response_XXXXXX";
	this->file_fd = mkstemp(&(*file_name.begin()));
	if (this->file_fd == ERROR)
			throw std::runtime_error("mkstemp(): " + (std::string)strerror(errno));
	this->file_stream.open(this->file_name.c_str(), std::fstream::in | std::fstream::out);
	if (this->file_stream.fail())
		throw std::ios_base::failure("failed to open file: " + this->file_name);
	// std::cout << "fd " << this->file_fd << " is created as " << this->file_name << std::endl; //DEBUG
}

void Response::createResponse() {
	std::map<std::string, ConfigParse> tmpConf(this->_config->getLocation());
	std::string tmpPath = this->_request->getPath();
	std::map<std::string, ConfigParse>::const_iterator it;

	// TMP = "/1.html"
	// TMP = "/inside/3.html"
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
	if (this->_request->getStatusCode() == 200)
	{
		if (this->_request->getMethod() == "GET")
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
	this->_response = this->_statusLine + this->_headers + this->_body;
}

// void	Response::setResponseLocation() {
// }

int	Response::httpGetMethod() {
	std::fstream file;

	if (isDirectory(this->_path) && this->_responseLocation->getAutoindex() == true)
	{
		DIR				*dp;
		struct dirent	*di_struct;
		struct stat		file_stats;

		this->_body = "<html><h1>Index of ";
		this->_body += this->_request->getPath() + "/</h1>";
		this->_body += "<table  style=\"width:100%; text-align:left; line-height: 2\">";
		this->_body += "<tr> <th>Name</th><th>Last modified</th><th>Size</th> </tr>";
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
				}
				this->_body += "</tr>";

			} 
		}
		this->_body += "</table></html>";
		removeDoubleSlash(this->_body);
		return 0;
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
	}
	else
		this->_request->setStatusCode(404);
	file.close();
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
			return 1;
		}
		file << fileData;
		file.close();

	}
	else
	{
		this->_request->setStatusCode(400);
		return 1;
	}
	std::cout << RESET << std::endl;
	return 0;
}

int Response::httpDeleteMethod() {
	// std::cout << YELLOW <<  "The Body is: "<< this->_request->getBody() << RESET << std::endl;
	return 0;
}

int		Response::setHeaders() {
	//TODO: Which Headers to Put in...
	this->_headers += "Version: " + this->_request->getVersion() + "\r\n";
	this->_headers += "Connection: keep-alive\r\n";
	// this->_headers += "Keep-Alive: timeout=10\r\n";
	this->_headers += "Content-Length: " + std::to_string(this->_body.length());
	this->_headers += "\r\n\r\n";

	return 0;
}

int 	Response::responseBodyErrorSet() {
	std::fstream errfile;
	std::string error_path;

	error_path = this->_config->getErrorPages()[this->_request->getStatusCode()];
	errfile.open(error_path, std::fstream::in | std::fstream::out);
	if (!errfile.fail())
	{
		std::string errbuff((std::istreambuf_iterator<char>(errfile)), std::istreambuf_iterator<char>());
		this->_body = errbuff + "\r\n";
	}
	else
	{
		this->_request->setStatusCode(500);
		this->_body = "<!doctype html><html><head><title>500 Internal Server Error</title><h1><b>Error 500</b></h1><h2>Internal Server Error</h2></head></html>\r\n";
	}
	errfile.close();
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
/* ************************************************************************** */
/*                             NO MEMBER FUNCTION                             */
/* ************************************************************************** */
