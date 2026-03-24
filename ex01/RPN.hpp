#ifndef RPN_HPP
#define RPN_HPP

#include <iostream>
#include <stack>

#define STD			"\033[0m"
#define PINK		"\033[38;5;199m\033[48;5;225m"
#define GREEN		"\033[38;5;34m\033[48;5;193m"

class RPN
{
	public:
		RPN();
		RPN(const RPN& other);
		RPN& operator=(const RPN& other);
		~RPN();
		
		void resolve(const std::string& expression);

	private:

		std::stack<int> _stack;
};

class expressionException : public std::exception
{
	public:
		expressionException(const std::string& msg): _msg(msg) {}
		virtual ~expressionException() throw() {}
		virtual const char* what() const throw()
		{
			return _msg.c_str();
		}
	private:
		std::string _msg;
};

#endif