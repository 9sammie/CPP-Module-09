#include "PmergeMe.hpp"
#include <climits>
#include <cerrno>
#include <sys/time.h>
#include <math.h>



//    🌲   🌲                 ✦  Constructors  ✦                  🌲   🌲    //
#pragma region

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
#pragma endregion



//    📕   📖                   ✦  parsing  ✦                     📖   📕    //
#pragma region

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
			throw std::runtime_error(("bad input: ") + *endPtr);
		if (val <= 0)
			throw std::runtime_error("negative number");

		_vec.push_back(static_cast<int>(val));
		_deq.push_back(static_cast<int>(val));
	}
}
#pragma endregion



//    📽️   🎞️                   ✦  display  ✦                     🎞️   📽️    //
#pragma region

void printDebug(std::vector<int> debug)
{
	if (debug.empty())
		return;
	std::cout << debug[0];
	for (int i = 1; i < debug.size(); ++i)
		std::cout << " " << debug[i];
	std::cout << std::endl;
}

void PmergeMe::printBefore()
{
	std::cout << _vec[0];
	for (int i = 1; i < _vec.size(); ++i)
		std::cout << " " << _vec[i];
	std::cout << std::endl;
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
	std::cout << "Time to process a range of " << size << " elements with"
			  << type << ": " << elapse << " us" << std::endl;
}
#pragma endregion



//    🛠️   🛠️                    ✦  utils  ✦                      🛠️   🛠️    //
#pragma region

static long long getTimeInMicro()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)tv.tv_sec * 1000000LL + (long long)tv.tv_usec;
}
#pragma endregion



//    🪵   🪵                 ✦  vectorLogic  ✦                   🪵   🪵    //
#pragma region

void PmergeMe::sortVector(int _elementSize)
{
	if (_elementSize * 2 > _vec.size())
		return;

	for (size_t i = 0; i + 2 * _elementSize <= _vec.size(); i += 2 * _elementSize)
	{
		size_t firstElementMaster = i + _elementSize -1;
		size_t secondElementMaster = i + 2 * _elementSize -1;

		if (_vec[firstElementMaster] > _vec[secondElementMaster])
			for (size_t k = 0; k < _elementSize; ++k)
				std::swap(_vec[i + k], _vec[i + _elementSize + k]);
	}

	sortVector(_elementSize * 2);


	// séparation de vec en main et pend
	std::vector<int> main;
	std::vector<int> pend;

	for (size_t k = 0; k < _elementSize; ++k)
		main.push_back(_vec[k]);
	for (size_t k = 0; k < _elementSize; ++k)
		main.push_back(_vec[_elementSize + k]);

	for (size_t i = 2 * _elementSize; i + 2 * _elementSize <= _vec.size(); i += 2 * _elementSize)
	{
		for (size_t k = 0; k < _elementSize; ++k)
			pend.push_back(_vec[i + k]);
		for (size_t k = 0; k < _elementSize; ++k)
			main.push_back(_vec[i + _elementSize + k]);
	}

	size_t processedSize = (_vec.size() / (2 * _elementSize)) * (2 * _elementSize); // ex: elementSize = 1 et _vec.size() = 17 --> ((17/(2*1)) * (2*1) = 16
	for (size_t i = processedSize; i < _vec.size(); ++i)
		pend.push_back(_vec[i]);


	// recherche des nombres de jacobsthal necessaire pour resoudre tout notre pend et l'inserer dans main.
	size_t pendElementNbr = pend.size() / _elementSize;

	std::vector<int> jacob;
	if (pendElementNbr == 0)
		return;

	jacob.push_back(3);
	jacob.push_back(5);
	while (jacob.back() < pendElementNbr)
	{
		size_t nextJacob = jacob[jacob.size() - 1] + 2 * jacob[jacob.size() - 2];
		jacob.push_back(nextJacob);
	}

	// Création de l'ordre d'insertion
	std::vector<int> insertionOrder;
	size_t lastJacob = 1;

	for (size_t i = 0; i < jacob.size(); i++)
	{
		size_t currentJacob = jacob[i];
		size_t startIdx = (currentJacob > pendElementNbr) ? pendElementNbr : currentJacob;

		for (size_t j = startIdx; j > lastJacob; --j) // ex: lastJacob = 5, then currJacob is 11 and we wanna go from 11 to 5.
			insertionOrder.push_back(j);
		lastJacob = currentJacob;
	}

	// proceder au binary insertion en comparant d'abord les pend element (b) indique par le vecteur jacob.
	for (size_t i = 0; i < insertionOrder.size(); ++i)
	{
		size_t itemIdx = insertionOrder[i];
		int elementMaster = pend[(itemIdx - 1) * _elementSize + _elementSize - 1]; // find the master element of the pend we want to compare with the master element of the main.
		size_t high;
		
		if (itemIdx * 2 * _elementSize > _vec.size())		// if itemIdx too big, he is an orphan, so we search in the entire main.
			high = main.size() / _elementSize;
		else												// else we index high on the main element master matching the pending one.
		{
			int mainElementMaster = _vec[(itemIdx * 2) * _elementSize - 1];
			high = 0;
			while (high < main.size() / _elementSize && main[high * _elementSize + _elementSize - 1] != mainElementMaster)
				high++;
		}

		size_t low = 0;
		while (low < high)
		{
			size_t mid = low + (high - low) / 2;
			if (main[mid * _elementSize -1] < elementMaster)
				low = mid + 1;
			else
				high = mid;
		}

		std::vector<int>::iterator insertPos = main.begin() + (low * _elementSize);
		std::vector<int>::iterator pendStart = pend.begin() + (itemIdx - 1) * _elementSize;
		std::vector<int>::iterator pendEnd = pendStart + _elementSize;

		main.insert(insertPos, pendStart, pendEnd);
	}

	// size_t totalExpected = _vec.size();
    // if (main.size() < totalExpected)
    // {
    //     size_t remainingInPend = totalExpected - main.size();
    //     size_t startByte = pend.size() - remainingInPend;
    //     for (size_t i = startByte; i < pend.size(); ++i)
    //         main.push_back(pend[i]);
    // }

	_vec = main;
}

#pragma endregion



//    ⛓️   ⛓️                 ✦  dequeLogic  ✦                   ⛓️   ⛓️    //
#pragma region
#pragma endregion



//    🏄‍♂️   🏄‍♂️                    ✦  launch  ✦                     🏄‍♂️   🏄‍♂️    //
#pragma region

void PmergeMe::launch()
{
	// _elementSize = 0;
	printBefore();

	long long startVec = getTimeInMicro();
	sortVector(1);
	long long endVec = getTimeInMicro();

	// long long startDeq = getTimeInMicro();
	// sortDeque(_deq);
	// long long endDeq = getTimeInMicro();

	printAfter();
	displayStats(" std::vector", static_cast<double>(endVec - startVec), _vec.size());
	// displayStats("std::deque", static_cast<double>(endDeq - startDeq), _deq.size());
}
#pragma endregion
