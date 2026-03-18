#include "BitcoinExchange.hpp"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Error : could not open file." << std::endl;
		return 1;
	}
	try
	{
		btc bitcoin("data.csv");
		bitcoin.processInput(argv[1]);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return 0;
}
