#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <exception>

class	GAI_EXCEPTION : public std::exception
{
	virtual const char *what() const throw() {return ("Error: getaddrinfo()");}
};

class	BIND_EXCEPTION : public std::exception
{
	virtual const char *what() const throw() {return ("Error: bind()");}
};

class	LISTEN_EXCEPTION : public std::exception
{
	virtual const char *what() const throw() {return ("Error: listen()");}
};

class	POLL_EXCEPTION : public std::exception
{
	virtual const char *what() const throw() {return ("Error: poll()");}
};

class	SHUTDOWN_EXCEPTION : public std::exception
{
	virtual const char *what() const throw() {return ("shutting down");}
};

#endif