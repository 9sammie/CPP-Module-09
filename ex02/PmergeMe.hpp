#ifndef PMERGERME_HPP
#define PMERGERME_HPP

#include <iostream>
#include <cctype>
#include <vector>
#include <deque>

class PmergeMe
{
	private:

		std::vector<int> _vec;
		std::deque<int> _deq;

		void sortVector(std::vector<int> _vec);
		void sortDeque(std::deque<int> _deq);
	
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