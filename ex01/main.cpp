#include "RPN.hpp"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./RPN <expression>" << std::endl;
        return 1;
    }

    RPN calculator;

	try
    {
        calculator.resolve(argv[1]);
    }
    catch (const std::exception& e)
    {
        std::cerr << PINK << "Error: " << e.what() << " " << STD << std::endl;
        return 1;
    }

    return 0;
}
