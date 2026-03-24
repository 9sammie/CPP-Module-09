#include "RPN.hpp"
#include <sstream>

RPN::RPN() : _stack() {}

RPN::RPN(const RPN& other)
{
    *this = other;
}

RPN& RPN::operator=(const RPN& other)
{
    if (this != &other)
        this->_stack = other._stack;
    return *this;
}

RPN::~RPN() {}

void RPN::resolve(const std::string& expression)
{
	std::istringstream iss(expression);
	std::string token;

	while (iss >> token)
	{
		if (token.length() != 1)
			throw expressionException("number over 9 detected");

		char c = token[0];

		if (isdigit(c))
			_stack.push(c - '0');

		else if (std::string("+-*/").find(c) != std::string::npos)
		{
			if (_stack.size() < 2)
				throw expressionException("missing a number to operate properly");

			int right = _stack.top(); _stack.pop();
			int left = _stack.top(); _stack.pop();
			int res = 0;

			if (c == '+') res = left + right;
			else if (c == '-') res = left - right;
			else if (c == '*') res = left * right;
			else if (c == '/')
			{
				if (right == 0)
					throw expressionException("division by zero detected");
				res = left / right;
			}
			_stack.push(res);
		}
		else
		{
			throw expressionException(std::string("invalid character found: ") + token[0]);
		}
	}

	if (_stack.size() != 1)
		throw expressionException("more than one number remaining");
	else
	{
		std::cout << GREEN << "⋆‧₊☽ " << _stack.top() << " ☾₊‧⋆" << STD << std::endl;
		_stack.pop();
	}
}
