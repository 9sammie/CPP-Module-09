#include "PmergeMe.hpp"
#include <climits>
#include <cerrno>
#include <sys/time.h>

PmergeMe::PmergeMe(): _vec(), _deq() {}

PmergeMe::PmergeMe(const PmergeMe& other): _vec(other._vec), _deq(other._deq) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
	if (this != &other)
	{
		_vec = other._vec;
		_deq = other._deq;
	}
	return *this;
}

PmergeMe::~PmergeMe() {}



void PmergeMe::parseInput(int ac, char** av)
{
	if (ac < 2)
		throw std::runtime_error("no input provided");

	for (int i = 1; i < ac; ++i)
	{
		if (av[i][0] == '\0')
            throw std::runtime_error("empty argument");

		char* endPtr;
		errno = 0;


		long val = std::strtol(av[i], &endPtr, 10);

		if (errno == ERANGE || val > INT_MAX || val < INT_MIN)
			throw std::runtime_error("integer overflow");
		if (*endPtr != '\0')
			throw std::runtime_error("bad input: " + *endPtr);
		if (val <= 0)
			throw std::runtime_error("negative number");


		_vec.push_back(static_cast<int>(val));
		_deq.push_back(static_cast<int>(val));
	}
}

void PmergeMe::printBefore()
{
	std::cout << _vec[0];
	for (int i = 1; i < _vec.size(); ++i)
		std::cout << " " << _vec[i];
	std::cout << std::endl;
}

static long long getTimeInMicro()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)tv.tv_sec * 1000000LL + (long long)tv.tv_usec;
}





void PmergeMe::printAfter()
{
	std::cout << _vec[0];
	for (int i = 1; i < _vec.size(); ++i)
		std::cout << " " << _vec[i];
	std::cout << std::endl;
}

static void displayStats(std::string type, double elapse, size_t size)
{
	std::cout << "Time to process a range of " << size << "elements with"
			  << type << ": " << elapse << " us" << std::endl;
}

void PmergeMe::launch()
{
	printBefore();

	long long startVec = getTimeInMicro();
	sortVector(_vec);
	long long endVec = getTimeInMicro();

	long long startDeq = getTimeInMicro();
	sortDeque(_deq);
	long long endDeq = getTimeInMicro();

	printAfter();
	displayStats("std::vector", static_cast<double>(endVec - startVec), _vec.size());
	displayStats("std::deque", static_cast<double>(endDeq - startDeq), _deq.size());
}
