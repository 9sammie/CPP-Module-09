#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <iostream>
#include <map>

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
		RuntimeException(const std::string& msg): _msg(msg) {}
		virtual ~RuntimeException() throw() {}
		virtual const char* what() const throw()
		{
			return _msg.c_str();
		}
	private:
		std::string _msg;
};

#endif