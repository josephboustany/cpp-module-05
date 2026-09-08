#include <iostream>
#include "Intern.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"

// Parallel arrays: _formNames[i] is the name makeForm() must be called with
// to get an instance created by _formCreators[i]. Keeping the two in sync by
// index avoids an if/elseif/else chain in makeForm() - adding a new form
// only means appending one entry to each array and one create* function.
const std::string Intern::_formNames[3] =
{
	"shrubbery creation",
	"robotomy request",
	"presidential pardon"
};

AForm *(*Intern::_formCreators[3])(std::string const &target) =
{
	&Intern::createShrubberyCreationForm,
	&Intern::createRobotomyRequestForm,
	&Intern::createPresidentialPardonForm
};

Intern::Intern()
{
	std::cout << "Intern default constructor called\n";
}

Intern::Intern(Intern const &other)
{
	(void)other;
	std::cout << "Intern copy constructor called\n";
}

Intern &Intern::operator=(Intern const &other)
{
	if (this != &other)
	{
		// Intern has no data members of its own to copy.
	}
	std::cout << "Intern copy assignment operator called\n";
	return *this;
}

Intern::~Intern()
{
	std::cout << "Intern destructor called\n";
}

AForm *Intern::createShrubberyCreationForm(std::string const &target)
{
	return new ShrubberyCreationForm(target);
}

AForm *Intern::createRobotomyRequestForm(std::string const &target)
{
	return new RobotomyRequestForm(target);
}

AForm *Intern::createPresidentialPardonForm(std::string const &target)
{
	return new PresidentialPardonForm(target);
}

AForm *Intern::makeForm(std::string const &formName, std::string const &target) const
{
	int formCount = static_cast<int>(sizeof(_formNames) / sizeof(_formNames[0]));

	for (int i = 0; i < formCount; i++)
	{
		if (_formNames[i] == formName)
		{
			std::cout << "Intern creates " << formName << "\n";
			return _formCreators[i](target);
		}
	}
	std::cout << "Intern could not find a form named \"" << formName << "\"\n";
	return NULL;
}
