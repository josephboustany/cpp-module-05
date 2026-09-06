#include <iostream>
#include <fstream>
#include "ShrubberyCreationForm.hpp"

ShrubberyCreationForm::ShrubberyCreationForm() : AForm("ShrubberyCreationForm", 145, 137), _target("")
{
	std::cout << "ShrubberyCreationForm default constructor called.\n";
}

ShrubberyCreationForm::ShrubberyCreationForm(std::string const &target) : AForm("ShrubberyCreationForm", 145, 137), _target(target)
{
	std::cout << "ShrubberyCreationForm constructor called for " << _target << ".\n";
}

ShrubberyCreationForm::ShrubberyCreationForm(ShrubberyCreationForm const &other) : AForm(other), _target(other._target)
{
	std::cout << "ShrubberyCreationForm copy constructor called.\n";
}

ShrubberyCreationForm &ShrubberyCreationForm::operator=(ShrubberyCreationForm const &other)
{
	if (this != &other)
		AForm::operator=(other);
	std::cout << "ShrubberyCreationForm copy assignment operator called.\n";
	return (*this);
}

ShrubberyCreationForm::~ShrubberyCreationForm()
{
	std::cout << "ShrubberyCreationForm destructor called.\n";
}

void ShrubberyCreationForm::executeAction() const
{
	std::ofstream file((_target + "_shrubbery").c_str());

	file << "        *\n"
			"       ***\n"
			"      *****\n"
			"     *******\n"
			"    *********\n"
			"   ***********\n"
			"       |||\n"
			"       |||\n";
	file.close();
	std::cout << "Shrubbery planted at " << _target << ".\n";
}
