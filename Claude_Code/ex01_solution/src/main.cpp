#include <iostream>
#include "Bureaucrat.hpp"
#include "Form.hpp"

int main()
{
	std::cout << "--- 1. default construction ---\n";
	{
		Bureaucrat defaultBureaucrat;
		Form defaultForm;
		std::cout << defaultBureaucrat;
		std::cout << defaultForm;
	}

	std::cout << "\n--- 2. valid Form construction + operator<< ---\n";
	{
		Form taxReturn("Tax Return", 50, 25);
		std::cout << taxReturn;
	}

	std::cout << "\n--- 3. Form construction with grade too high (0) ---\n";
	try
	{
		Form tooHigh("TooHigh", 0, 50);
		(void)tooHigh;
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}

	std::cout << "\n--- 4. Form construction with grade too low (151) ---\n";
	try
	{
		Form tooLow("TooLow", 50, 151);
		(void)tooLow;
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}

	std::cout << "\n--- 5. beSigned() succeeds (grade high enough) ---\n";
	{
		Bureaucrat highRank("HighRank", 1);
		Form form("Permit", 10, 5);
		form.beSigned(highRank);
		std::cout << form;
	}

	std::cout << "\n--- 6. beSigned() fails (grade too low) ---\n";
	{
		Bureaucrat lowRank("LowRank", 100);
		Form form("Permit", 10, 5);
		try
		{
			form.beSigned(lowRank);
		}
		catch (std::exception& e)
		{
			std::cout << "Caught: " << e.what() << "\n";
		}
		std::cout << form;
	}

	std::cout << "\n--- 7. signForm() success message ---\n";
	{
		Bureaucrat boss("Boss", 1);
		Form request("Request", 10, 5);
		boss.signForm(request);
		std::cout << request;
	}

	std::cout << "\n--- 8. signForm() failure message ---\n";
	{
		Bureaucrat intern("Intern", 150);
		Form request("Request", 10, 5);
		intern.signForm(request);
		std::cout << request;
	}

	std::cout << "\n--- 9. copy constructor and copy assignment ---\n";
	{
		Bureaucrat signer("Signer", 1);
		Form original("Original", 10, 5);
		original.beSigned(signer);

		Form copyCtor(original);
		std::cout << copyCtor;

		Form assigned("Assigned", 99, 99);
		assigned = original;
		std::cout << assigned;
		std::cout << "Note: names/grades differ on purpose (" << assigned.getName()
			<< " vs " << original.getName() << ") because only _isSigned"
			" is copied by operator=; the rest is const.\n";
	}

	std::cout << "\n--- end of main ---\n";
	return 0;
}
