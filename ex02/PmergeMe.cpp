#include "PmergeMe.hpp"
#include <climits>
#include <cerrno>
#include <sys/time.h>
#include <math.h>
#include <iomanip>



//                            ✦  Constructors  ✦                             //
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



//                               ✦  parsing  ✦                                //
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

		long val = strtol(av[i], &endPtr, 10);

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



//                               ✦  display  ✦                                //
#pragma region

static long long getTimeInMicro()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)tv.tv_sec * 1000000LL + (long long)tv.tv_usec;
}

void PmergeMe::printBefore()
{
	std::cout << _vec[0];
	for (size_t i = 1; i < _vec.size(); ++i)
		std::cout << " " << _vec[i];
	std::cout << std::endl;
}

void PmergeMe::printAfter()
{
	std::cout << _vec[0];
	for (size_t i = 1; i < _vec.size(); ++i)
		std::cout << " " << _vec[i];
	std::cout << std::endl;
}

static void displayStats(std::string type, double elapse, int size)
{
	std::cout << "Time to process a range of " << size << " elements with"
			  << type << ": " << std::fixed << std::setprecision(5) << elapse << " us" << std::endl;
}
#pragma endregion



//                             ✦  vectorLogic  ✦                              //
#pragma region

void PmergeMe::vInsertOrphan(int _elementSize, std::vector<int> *main)
{
	size_t totalProcessed = main->size();

	if (totalProcessed < _vec.size())
	{
	    for (size_t i = totalProcessed; i < _vec.size(); i += _elementSize)
		{
	        int currentBlockSize = std::min(_elementSize, static_cast<int>(_vec.size() - i));

			int masterValue = _vec[i + currentBlockSize - 1];

	        size_t low = 0;
	        size_t high = main->size() / _elementSize;

	        while (low < high)
			{
	            size_t mid = low + (high - low) / 2;
	            if ((*main)[mid * _elementSize - 1] < masterValue)
	                low = mid + 1;
	            else
	                high = mid;
	        }
			std::vector<int>::iterator itPos = main->begin() + (low * _elementSize);
			std::vector<int>::const_iterator itStart = _vec.begin() + i;
			std::vector<int>::const_iterator itEnd = itStart + currentBlockSize;

			main->insert(itPos, itStart, itEnd);
		}
	}
}

void PmergeMe::vBinaryInsertion(int _elementSize, std::vector<int> insertionOrder, std::vector<int> *main, std::vector<int> pend)
{
	printf(", insertion order: ");
	for(size_t i = 0; i < insertionOrder.size(); i++)
		printf("%d ", insertionOrder[i]);

	printf(", _vec: ");
	for(size_t i = 0; i < _vec.size(); i++)
		printf("%d ", _vec[i]);

	printf(", main: ");
	for(size_t i = 0; i < main->size(); i++)
		printf("%d ", (*main)[i]);
		
	printf(", pend: ");
	for(size_t i = 0; i < pend.size(); i++)
		printf("%d ", pend[i]);		

	for (size_t i = 0; i < insertionOrder.size(); i++)
	{
		// 1/ search for the an of the bn (the pair)
		int itemIdx = insertionOrder[i];

		size_t pendOffset = (itemIdx - 2) * _elementSize;
		if (pendOffset >= pend.size())
			continue;

		int pendElementMaster = pend[pendOffset + _elementSize - 1]; // find the master element of the pend we want to compare with the master element of the main.
		size_t high;

		if (itemIdx * 2 * _elementSize > static_cast<int>(_vec.size()))		// if itemIdx too big, he is an orphan, so we search in the entire main.
		{
			high = main->size() / _elementSize;
			printf(", high: %zu", high);
		}
		else												// else we index high on the main element master matching the pending one.
		{
			int mainElementMaster = _vec[(itemIdx * 2) * _elementSize - 1];
			high = 0;
			while (high < main->size() / _elementSize && (*main)[high * _elementSize + _elementSize - 1] != mainElementMaster)
				high++;
			printf(", mainMaster of mainPend(%d): %d, high: %zu", pendElementMaster, mainElementMaster, high);
		}

		// 2/ insert from the matching pair since we know an is bigger than bn
		size_t low = 0;
		while (low < high)
		{
			int mid = low + (high - low) / 2;
			if ((*main)[(mid + 1)* _elementSize -1] < pendElementMaster)
				low = mid + 1;
			else
				high = mid;
		}

		std::vector<int>::iterator insertPos = main->begin() + (low * _elementSize);
		std::vector<int>::iterator pendStart = pend.begin() + (itemIdx - 2) * _elementSize;

		int remainingElements = std::min(_elementSize, static_cast<int>(pend.size() - pendOffset));
		std::vector<int>::iterator pendEnd = pendStart + remainingElements;

		main->insert(insertPos, pendStart, pendEnd);
	}
}

void PmergeMe::vFindInsertionOrder(int _elementSize, std::vector<int> *insertionOrder, std::vector<int> jacob, int pendSize)
{
	int maxBNbr = (pendSize / _elementSize + 1);
	int pendElementNbr = pendSize / _elementSize;

	int lastJacob = 1;

	for (size_t i = 0; i < jacob.size(); i++)
	{
		int currentJacob = jacob[i];
		int startIdx = (currentJacob > maxBNbr) ? maxBNbr : currentJacob;

		for (int j = startIdx; j > lastJacob; --j) // ex: lastJacob = 5, then currJacob is 11 and we wanna go from 11 to 5.
			insertionOrder->push_back(j);
		if (currentJacob >= pendElementNbr)
	        break;
		lastJacob = currentJacob;
	}
}

void PmergeMe::vFindJacobNbrs(int _elementSize, std::vector<int> *jacob, int pendSize)
{
	int pendElementNbr = pendSize / _elementSize;

	if (pendElementNbr == 0)
		return;

	jacob->push_back(3);
	jacob->push_back(5);
	while (jacob->back() < pendElementNbr)
	{
		int nextJacob = (*jacob)[jacob->size() - 1] + 2 * (*jacob)[jacob->size() - 2];
		jacob->push_back(nextJacob);
	}
	printf("jacob: ");
	for(size_t i = 0; i < jacob->size(); i++)
		printf("%d ", (*jacob)[i]);
}

void PmergeMe::vInitVectors(int _elementSize, std::vector<int> *main, std::vector<int> *pend)
{
	for (int k = 0; k < _elementSize; ++k) main->push_back(_vec[k]);
	for (int k = 0; k < _elementSize; ++k) main->push_back(_vec[_elementSize + k]);

	for (int i = 2 * _elementSize; i + 2 * _elementSize <= static_cast<int>(_vec.size()); i += 2 * _elementSize)
	{
		for (int k = 0; k < _elementSize; ++k) pend->push_back(_vec[i + k]);
		for (int k = 0; k < _elementSize; ++k) main->push_back(_vec[i + _elementSize + k]);
	}

	int processedSize = (_vec.size() / (2 * _elementSize)) * (2 * _elementSize); // ex: elementSize = 1 et _vec.size() = 17 --> ((17/(2*1)) * (2*1) = 16
	for (size_t i = processedSize; i < _vec.size(); ++i)
		pend->push_back(_vec[i]);
}

bool PmergeMe::vPairAndSwap(int _elementSize)
{
	if (_elementSize * 2 > static_cast<int>(_vec.size()))
		return 1;

	for (int i = 0; i + 2 * _elementSize <= static_cast<int>(_vec.size()); i += 2 * _elementSize)
	{
		int firstElementMaster = i + _elementSize -1;
		int secondElementMaster = i + 2 * _elementSize -1;

		if (_vec[firstElementMaster] > _vec[secondElementMaster])
			for (int k = 0; k < _elementSize; ++k)
				std::swap(_vec[i + k], _vec[i + _elementSize + k]);
	}
	return 0;
}

void PmergeMe::sortVector(int _elementSize)
{
	if (vPairAndSwap(_elementSize))
		return;

	sortVector(_elementSize * 2);

	std::vector<int> main;
	std::vector<int> pend;
	std::vector<int> jacob;
	std::vector<int> insertionOrder;

	vInitVectors(_elementSize, &main, &pend);

	vFindJacobNbrs(_elementSize, &jacob, pend.size());

	vFindInsertionOrder(_elementSize, &insertionOrder, jacob, pend.size());

	vBinaryInsertion(_elementSize, insertionOrder, &main, pend);

	vInsertOrphan(_elementSize, &main);

	_vec = main;
	printf("\n");
}

#pragma endregion



//                             ✦  dequeLogic  ✦                              //
#pragma region

void PmergeMe::dInsertOrphan(std::deque<int> *main)
{
	if (main->size() < _deq.size())
	{
	    for (size_t i = main->size(); i < _deq.size(); ++i) {
	        int orphan = _deq[i];

	        int low = 0;
	        int high = main->size();
	        while (low < high) {
	            int mid = low + (high - low) / 2;
	            if ((*main)[mid] < orphan)
	                low = mid + 1;
	            else
	                high = mid;
	        }
	        main->insert(main->begin() + low, orphan);
	    }
	}
}

void PmergeMe::dBinaryInsertion(int _elementSize, std::deque<int> insertionOrder, std::deque<int> *main, std::deque<int> pend)
{
	for (size_t i = 0; i < insertionOrder.size(); i++)
	{
		// 1/ search for the an of the bn (the pair)
		int itemIdx = insertionOrder[i];

		size_t pendOffset = (itemIdx - 2) * _elementSize;
		if (pendOffset >= pend.size())
			continue;

		int pendElementMaster = pend[pendOffset + _elementSize - 1]; // find the master element of the pend we want to compare with the master element of the main.
		size_t high;

		if (itemIdx * 2 * _elementSize > static_cast<int>(_deq.size()))		// if itemIdx too big, he is an orphan, so we search in the entire main.
			high = main->size() / _elementSize;
		else												// else we index high on the main element master matching the pending one.
		{
			int mainElementMaster = _deq[(itemIdx * 2) * _elementSize - 1];
			high = 0;
			while (high < main->size() / _elementSize && (*main)[high * _elementSize + _elementSize - 1] != mainElementMaster)
				high++;
		}

		// 2/ insert from the matching pair since we know an is bigger than bn
		size_t low = 0;
		while (low < high)
		{
			size_t mid = low + (high - low) / 2;
			if ((*main)[(mid + 1)* _elementSize -1] < pendElementMaster)
				low = mid + 1;
			else
				high = mid;
		}

		std::deque<int>::iterator insertPos = main->begin() + (low * _elementSize);
		std::deque<int>::iterator pendStart = pend.begin() + (itemIdx - 2) * _elementSize;

		int remainingElements = std::min(_elementSize, (int)(pend.size() - pendOffset));
		std::deque<int>::iterator pendEnd = pendStart + remainingElements;

		main->insert(insertPos, pendStart, pendEnd);
	}
}

void PmergeMe::dFindInsertionOrder(int _elementSize, std::deque<int> *insertionOrder, std::deque<int> jacob, int pendSize)
{
	int maxBNbr = (pendSize / _elementSize + 1);
	int pendElementNbr = pendSize / _elementSize;

	int lastJacob = 1;

	for (size_t i = 0; i < jacob.size(); i++)
	{
		int currentJacob = jacob[i];
		int startIdx = (currentJacob > maxBNbr) ? maxBNbr : currentJacob;

		for (int j = startIdx; j > lastJacob; --j) // ex: lastJacob = 5, then currJacob is 11 and we wanna go from 11 to 5.
			insertionOrder->push_back(j);
		if (currentJacob >= pendElementNbr)
	        break;
		lastJacob = currentJacob;
	}
}

void PmergeMe::dFindJacobNbrs(int _elementSize, std::deque<int> *jacob, int pendSize)
{
	int pendElementNbr = pendSize / _elementSize;

	if (pendElementNbr == 0)
		return;

	jacob->push_back(3);
	jacob->push_back(5);
	while (jacob->back() < pendElementNbr)
	{
		int nextJacob = (*jacob)[jacob->size() - 1] + 2 * (*jacob)[jacob->size() - 2];
		jacob->push_back(nextJacob);
	}
}

void PmergeMe::dInitdeques(int _elementSize, std::deque<int> *main, std::deque<int> *pend)
{
	for (int k = 0; k < _elementSize; ++k) main->push_back(_deq[k]);
	for (int k = 0; k < _elementSize; ++k) main->push_back(_deq[_elementSize + k]);

	for (size_t i = 2 * _elementSize; i + 2 * _elementSize <= _deq.size(); i += 2 * _elementSize)
	{
		for (int k = 0; k < _elementSize; ++k) pend->push_back(_deq[i + k]);
		for (int k = 0; k < _elementSize; ++k) main->push_back(_deq[i + _elementSize + k]);
	}

	int processedSize = (_deq.size() / (2 * _elementSize)) * (2 * _elementSize); // ex: elementSize = 1 et _deq.size() = 17 --> ((17/(2*1)) * (2*1) = 16
	for (size_t i = processedSize; i < _deq.size(); ++i)
		pend->push_back(_deq[i]);
}

bool PmergeMe::dPairAndSwap(int _elementSize)
{
	if (_elementSize * 2 > static_cast<int>(_deq.size()))
		return 1;

	for (size_t i = 0; i + 2 * _elementSize <= _deq.size(); i += 2 * _elementSize)
	{
		int firstElementMaster = i + _elementSize -1;
		int secondElementMaster = i + 2 * _elementSize -1;

		if (_deq[firstElementMaster] > _deq[secondElementMaster])
			for (int k = 0; k < _elementSize; ++k)
				std::swap(_deq[i + k], _deq[i + _elementSize + k]);
	}
	return 0;
}

void PmergeMe::sortDeque(int _elementSize)
{
	if (dPairAndSwap(_elementSize))
		return;

	sortDeque(_elementSize * 2);

	std::deque<int> main;
	std::deque<int> pend;
	std::deque<int> jacob;
	std::deque<int> insertionOrder;

	dInitdeques(_elementSize, &main, &pend);

	dFindJacobNbrs(_elementSize, &jacob, pend.size());

	dFindInsertionOrder(_elementSize, &insertionOrder, jacob, pend.size());

	dBinaryInsertion(_elementSize, insertionOrder, &main, pend);

	dInsertOrphan(&main);

	_deq = main;
}

#pragma endregion



//                               ✦  launch  ✦                                //
#pragma region

void PmergeMe::launch()
{
	printBefore();

	long long startVec = getTimeInMicro();
	sortVector(1);
	long long endVec = getTimeInMicro();

	// long long startDeq = getTimeInMicro();
	// sortDeque(1);
	// long long endDeq = getTimeInMicro();

	printAfter();
	displayStats(" std::vector", static_cast<double>(endVec - startVec)*0.00001, _vec.size());
	// displayStats(" std::deque", static_cast<double>(endDeq - startDeq)*0.00001, _deq.size());
}
#pragma endregion
