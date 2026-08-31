#include <iostream>
#include "Bureaucrat.hpp"

int main()
{
	std::cout << "--- 1. valid construction + operator<< ---\n";
	{
		Bureaucrat bob("Bob", 42);
		std::cout << bob;
	}

	std::cout << "\n--- 2. construction with grade too high (0) ---\n";
	try
	{
		Bureaucrat tooHigh("TooHigh", 0);
		(void)tooHigh;
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}

	std::cout << "\n--- 3. construction with grade too low (151) ---\n";
	try
	{
		Bureaucrat tooLow("TooLow", 151);
		(void)tooLow;
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}

	std::cout << "\n--- 4. increment / decrement within range ---\n";
	{
		Bureaucrat alice("Alice", 3);
		std::cout << alice;
		alice.incrementGrade();
		std::cout << alice;
		alice.decrementGrade();
		alice.decrementGrade();
		std::cout << alice;
	}

	std::cout << "\n--- 5. incrementGrade() past grade 1 throws ---\n";
	try
	{
		Bureaucrat topDog("TopDog", 1);
		topDog.incrementGrade();
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}

	std::cout << "\n--- 6. decrementGrade() past grade 150 throws ---\n";
	try
	{
		Bureaucrat bottomFeeder("BottomFeeder", 150);
		bottomFeeder.decrementGrade();
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}

	std::cout << "\n--- 7. copy constructor and copy assignment ---\n";
	{
		Bureaucrat original("Original", 10);
		Bureaucrat copyCtor(original);
		std::cout << copyCtor;

		Bureaucrat assigned("Assigned", 99);
		assigned = original;
		std::cout << assigned;
		std::cout << "Note: names differ on purpose (" << assigned.getName()
			<< " vs " << original.getName() << ") because _name is const"
			" and is not copied by operator=.\n";
	}

	std::cout << "\n--- end of main ---\n";
	return 0;
}
