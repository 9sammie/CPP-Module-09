#include "BitcoinExchange.hpp"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << PINK << "Error : could not open file." << STD << std::endl;
		return 1;
	}
	try
	{
		btc bitcoin("data.csv");
		bitcoin.processInput(argv[1]);
	}
	catch (std::exception &e)
	{
		std::cerr << PINK << "Error: " << e.what() << STD << std::endl;
	}
	return 0;
}
