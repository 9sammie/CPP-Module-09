#ifndef PMERGERME_HPP
#define PMERGERME_HPP

#include <iostream>
#include <cctype>
#include <vector>
#include <deque>
#include <stdio.h>

class PmergeMe
{
	private:

		std::vector<int> _vec;
		std::deque<int> _deq;

		void sortVector(int _elementSize);
		bool vPairAndSwap(int _elementSize);
		void vInitVectors(int _elementSize, std::vector<int> *main, std::vector<int> *pend);
		void vFindJacobNbrs(int _elementSize, std::vector<int> *jacob, int pendSize);
		void vFindInsertionOrder(int _elementSize, std::vector<int> *insertionOrder, std::vector<int> jacob, int pendSize);
		void vBinaryInsertion(int _elementSize, std::vector<int> insertionOrder, std::vector<int> *main, std::vector<int> pend);
		void vInsertOrphan(int _elementSize, std::vector<int> *main);


		void sortDeque(int _elementSize);
		bool dPairAndSwap(int _elementSize);
		void dInitdeques(int _elementSize, std::deque<int> *main, std::deque<int> *pend);
		void dFindJacobNbrs(int _elementSize, std::deque<int> *jacob, int pendSize);
		void dFindInsertionOrder(int _elementSize, std::deque<int> *insertionOrder, std::deque<int> jacob, int pendSize);
		void dBinaryInsertion(int _elementSize, std::deque<int> insertionOrder, std::deque<int> *main, std::deque<int> pend);
		void dInsertOrphan(std::deque<int> *main);
	
	public:

		PmergeMe();
		PmergeMe(const PmergeMe &other);
		PmergeMe &operator=(const PmergeMe &other);
		~PmergeMe();

		void parseInput(int ac, char** av);
		void launch();
		void printBefore();
		void printAfter();
};

#endif