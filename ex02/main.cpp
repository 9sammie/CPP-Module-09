#include "PmergeMe.hpp"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Error: valid arguments required" << std::endl;
		return 1;
	}

	PmergeMe sort;

	try
	{
		sort.parseInput(argc, argv);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		return 1;
	}
	return 0;
}
