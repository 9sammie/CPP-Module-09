#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <iostream>
#include <map>

#define STD				"\033[0m"
#define GREEN			"\033[38;5;34m\033[48;5;193m"
#define PINK			"\033[38;5;199m\033[48;5;225m"

class btc
{
	public:
		btc();
		btc(std::string file);
		btc(const btc& other);
		btc& operator=(const btc& other);
		~btc();

		std::map<std::string, float>& getMap();

		bool isValidDate(const std::string& date);
		bool isValidValue(const std::string& str);
		float getExchangeRate(std::string date);
		std::string trim(const std::string& s);
		void processInput(std::string filename);
	private:
		std::map<std::string, float> _tab;
};

class RuntimeException : public std::exception
{
	public:
		RuntimeException(const std::string& msg) 
		{
			_msg = msg + " " + STD;
		}
		virtual ~RuntimeException() throw() {}
		virtual const char* what() const throw()
		{
			return _msg.c_str();
		}
	private:
		std::string _msg;
};

#endif