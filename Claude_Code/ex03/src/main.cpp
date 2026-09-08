#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "Bureaucrat.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include "Intern.hpp"

int main()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	std::cout << "--- 1. default construction of each concrete form ---\n";
	{
		ShrubberyCreationForm shrub;
		RobotomyRequestForm robot;
		PresidentialPardonForm pardon;
		std::cout << shrub;
		std::cout << robot;
		std::cout << pardon;
	}

	std::cout << "\n--- 2. named/target construction + operator<< ---\n";
	{
		ShrubberyCreationForm shrub("garden");
		RobotomyRequestForm robot("Bender");
		PresidentialPardonForm pardon("Trillian");
		std::cout << shrub;
		std::cout << robot;
		std::cout << pardon;
	}

	std::cout << "\n--- 3. beSigned() succeeds (grade high enough) ---\n";
	{
		Bureaucrat highRank("HighRank", 1);
		PresidentialPardonForm form("Permit");
		form.beSigned(highRank);
		std::cout << form;
	}

	std::cout << "\n--- 4. beSigned() fails (grade too low) ---\n";
	{
		Bureaucrat lowRank("LowRank", 100);
		PresidentialPardonForm form("Permit");
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

	std::cout << "\n--- 5. signForm() success message ---\n";
	{
		Bureaucrat boss("Boss", 1);
		PresidentialPardonForm request("Request");
		boss.signForm(request);
		std::cout << request;
	}

	std::cout << "\n--- 6. signForm() failure message ---\n";
	{
		Bureaucrat intern("Intern", 150);
		PresidentialPardonForm request("Request");
		intern.signForm(request);
		std::cout << request;
	}

	std::cout << "\n--- 7. copy constructor and copy assignment ---\n";
	{
		Bureaucrat signer("Signer", 1);
		PresidentialPardonForm original("Original");
		original.beSigned(signer);

		PresidentialPardonForm copyCtor(original);
		std::cout << copyCtor;

		PresidentialPardonForm assigned("Assigned");
		assigned = original;
		std::cout << assigned;
		std::cout << "Note: names/targets differ on purpose (" << assigned.getName()
			<< ") because only _isSigned (inherited from AForm) is copied by"
			" operator=; the rest, including the derived class's own _target,"
			" is const.\n";
	}

	std::cout << "\n--- 8. Bureaucrat construction with invalid grade ---\n";
	try
	{
		Bureaucrat tooHigh("TooHigh", 0);
		(void)tooHigh;
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}
	try
	{
		Bureaucrat tooLow("TooLow", 151);
		(void)tooLow;
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}

	std::cout << "\n--- 9. incrementGrade() / decrementGrade() ---\n";
	{
		Bureaucrat mid("Mid", 3);
		std::cout << mid;
		mid.incrementGrade();
		std::cout << mid;
		mid.decrementGrade();
		mid.decrementGrade();
		std::cout << mid;
	}
	try
	{
		Bureaucrat topDog("TopDog", 1);
		topDog.incrementGrade();
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}
	try
	{
		Bureaucrat bottomFeeder("BottomFeeder", 150);
		bottomFeeder.decrementGrade();
	}
	catch (std::exception& e)
	{
		std::cout << "Caught: " << e.what() << "\n";
	}

	std::cout << "\n--- 10. execute() on an unsigned form throws FormNotSignedException ---\n";
	{
		Bureaucrat boss("Boss", 1);
		ShrubberyCreationForm shrub("unsigned_garden");
		try
		{
			shrub.execute(boss);
		}
		catch (std::exception& e)
		{
			std::cout << "Caught: " << e.what() << "\n";
		}
	}

	std::cout << "\n--- 11. signed, but executor's grade is too low to execute ---\n";
	{
		Bureaucrat signer("Signer", 1);
		Bureaucrat weakExecutor("WeakExecutor", 150);
		PresidentialPardonForm pardon("Arthur Dent");
		signer.signForm(pardon);
		std::cout << pardon;
		try
		{
			pardon.execute(weakExecutor);
		}
		catch (std::exception& e)
		{
			std::cout << "Caught: " << e.what() << "\n";
		}
	}

	std::cout << "\n--- 12. Bureaucrat::executeForm() success and failure messages ---\n";
	{
		Bureaucrat boss("Boss", 1);
		PresidentialPardonForm pardon("Ford Prefect");
		boss.signForm(pardon);
		boss.executeForm(pardon);

		PresidentialPardonForm unsignedPardon("Marvin");
		boss.executeForm(unsignedPardon);
	}

	std::cout << "\n--- 13. ShrubberyCreationForm actually writes a file ---\n";
	{
		Bureaucrat boss("Boss", 1);
		ShrubberyCreationForm shrub("backyard");
		boss.signForm(shrub);
		boss.executeForm(shrub);

		std::ifstream check("backyard_shrubbery");
		if (check.is_open())
		{
			std::cout << "backyard_shrubbery contents:\n";
			std::string line;
			while (std::getline(check, line))
				std::cout << line << "\n";
		}
		else
			std::cout << "backyard_shrubbery could not be opened.\n";
	}

	std::cout << "\n--- 14. RobotomyRequestForm: run a few times to see both outcomes ---\n";
	{
		Bureaucrat boss("Boss", 1);
		for (int i = 0; i < 4; i++)
		{
			RobotomyRequestForm robot("subject");
			boss.signForm(robot);
			boss.executeForm(robot);
		}
	}

	std::cout << "\n--- 15. Intern::makeForm() creates each known form type ---\n";
	{
		Intern someRandomIntern;
		Bureaucrat boss("Boss", 1);

		AForm *shrub = someRandomIntern.makeForm("shrubbery creation", "backyard");
		AForm *robot = someRandomIntern.makeForm("robotomy request", "Bender");
		AForm *pardon = someRandomIntern.makeForm("presidential pardon", "Marvin");

		if (shrub)
		{
			std::cout << *shrub;
			boss.signForm(*shrub);
			boss.executeForm(*shrub);
			delete shrub;
		}
		if (robot)
		{
			std::cout << *robot;
			boss.signForm(*robot);
			boss.executeForm(*robot);
			delete robot;
		}
		if (pardon)
		{
			std::cout << *pardon;
			boss.signForm(*pardon);
			boss.executeForm(*pardon);
			delete pardon;
		}
	}

	std::cout << "\n--- 16. Intern::makeForm() with an unknown form name ---\n";
	{
		Intern someRandomIntern;
		AForm *unknown = someRandomIntern.makeForm("28C form", "Bender");
		if (unknown)
			delete unknown;
	}

	std::cout << "\n--- 17. Intern copy constructor and copy assignment ---\n";
	{
		Intern original;
		Intern copyCtor(original);
		Intern assigned;
		assigned = original;
		(void)copyCtor;
	}

	std::cout << "\n--- end of main ---\n";
	return 0;
}
