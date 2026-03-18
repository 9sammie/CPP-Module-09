#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <float.h>
#include <errno.h>

btc::btc(): _tab() {}

btc::btc(std::string file)
{
	std::ifstream ifs(file.c_str());
	if (!ifs.is_open())
	{
		throw RuntimeException("error opening file/file does not exist");
		return;
	}
	std::string line;
	std::getline(ifs, line);

	while (std::getline(ifs, line))
	{
		if (line.empty()) continue;

		std::stringstream ss(line);
		std::string date;
		std::string valueStr;

		if (line.c_str()[10] != ',' || !line.c_str()[11])
			throw RuntimeException("Error: corrupted database at line: " + line);
		if (std::getline(ss, date, ',') && std::getline(ss, valueStr))
		{
			if (!isValidDate(date) || !isValidValue(valueStr))
				throw RuntimeException("Error: corrupted database at line: " + line);
			float valueFlt = atof(valueStr.c_str());
			_tab[date] = valueFlt;
		}
	}
	ifs.close();
}

btc::btc(const btc& other): _tab(other._tab) {}

btc& btc::operator=(const btc& other)
{
	if (this != &other)
		_tab = other._tab;
	return *this;
}

btc::~btc() {}





std::map<std::string, float>& btc::getMap()
{
	return _tab;
}





bool btc::isValidDate(const std::string& date)
{
	int year = atoi(date.substr(0, 4).c_str());
	int month = atoi(date.substr(5, 2).c_str());
	int day = atoi(date.substr(8, 2).c_str());

	if (date.size() != 10)
		return false;
	if (date[4] != '-' || date[7] != '-')
		return false;
	
	if (year > 2025 || year < 2009 || month < 1 || month > 12 || day < 1 || day > 31)
		return false;
	
	if (month == 4 || month == 6 || month == 9 || month == 11)
	{
		if (day > 30)
			return false;
	}

	if (month == 2)
	{
		bool isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
		if (isLeap && day > 29)
			return false;
		if (!isLeap && day > 28)
			return false;
	}
	return true;
}

bool btc::isValidValue(const std::string& str)
{
	if (str.empty()) return false;

	errno = 0;
	char* endPtr;
	double val = strtod(str.c_str(), &endPtr);

	if (endPtr == str.c_str()) return false;

	while (*endPtr != '\0')
	{
		if (!isspace(*endPtr)) return false;
		endPtr++;
	}

	if (errno == ERANGE) return false;

	if (val < 0 || val > FLT_MAX) return false;

	return true;
}

// soit on trouve la date exact, soit on renvoie celle avant la plus
// proche superieur (la plus proche inferieur). 
float btc::getExchangeRate(std::string date)
{
	std::map<std::string, float>::iterator it = _tab.lower_bound(date);
	if (it != _tab.end() && it->first == date)
		return it->second;
	if (it == _tab.begin())
		return 0;
	--it;
	return it->second;
}

std::string btc::trim(const std::string& s)
{
	size_t first = s.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) return "";
	size_t last = s.find_last_not_of(" \t\n\r");
	return s.substr(first, (last - first + 1));
}

void btc::processInput(std::string filename)
{
	std::ifstream ifs (filename.c_str());
	if (!ifs.is_open())
	{
		std::cerr << "Error: could not open file." << std::endl;
		return;
	}

	std::string line;
	std::getline(ifs, line);

	while (std::getline(ifs, line))
	{
		if (line.empty())
			continue;
		
		size_t sep = line.find('|');
		if (sep == std::string::npos)
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		std::string date = trim(line.substr(0, sep));
		std::string valStr = trim(line.substr(sep + 1));

		if (!isValidDate(date))
		{
			std::cerr << "Error: bad input => " << date << std::endl;
			continue;
		}
		char* endptr;
		float userValue = strtod(valStr.c_str(), &endptr);
		if (*endptr != '\0' && !isspace(*endptr))
		{
			std::cerr << "Error: bad input => " << valStr << std::endl;
			continue;
		}
		if (userValue < 0)
		{
			std::cerr << "Error: not a positive number." << std::endl;
			continue;
		}
		if (userValue > 1000)
		{
			std::cerr << "Error: too large number." << std::endl;
			continue;
		}
		float price = getExchangeRate(date);
		std::cout << date << " => " << userValue << " = " << (userValue * price) << std::endl;
	}
}
