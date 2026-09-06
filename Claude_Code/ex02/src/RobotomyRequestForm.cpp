#include <iostream>
#include <cstdlib>
#include "RobotomyRequestForm.hpp"

RobotomyRequestForm::RobotomyRequestForm() : AForm("RobotomyRequestForm", 72, 45), _target("")
{
	std::cout << "RobotomyRequestForm default constructor called.\n";
}

RobotomyRequestForm::RobotomyRequestForm(std::string const &target) : AForm("RobotomyRequestForm", 72, 45), _target(target)
{
	std::cout << "RobotomyRequestForm constructor called for " << _target << ".\n";
}

RobotomyRequestForm::RobotomyRequestForm(RobotomyRequestForm const &other) : AForm(other), _target(other._target)
{
	std::cout << "RobotomyRequestForm copy constructor called.\n";
}

RobotomyRequestForm &RobotomyRequestForm::operator=(RobotomyRequestForm const &other)
{
	if (this != &other)
		AForm::operator=(other);
	std::cout << "RobotomyRequestForm copy assignment operator called.\n";
	return (*this);
}

RobotomyRequestForm::~RobotomyRequestForm()
{
	std::cout << "RobotomyRequestForm destructor called.\n";
}

void RobotomyRequestForm::executeAction() const
{
	std::cout << "* drilling noises *\n";
	if (rand() % 2 == 0)
		std::cout << _target << " has been robotomized successfully.\n";
	else
		std::cout << _target << " robotomy failed.\n";
}
