#include <iostream>
#include "AForm.hpp"
#include "Bureaucrat.hpp"

const int AForm::_gradeMin = 1;
const int AForm::_gradeMax = 150;

AForm::AForm() : _name(""), _isSigned(false), _gradeToSign(_gradeMax), _gradeToExecute(_gradeMax)
{
	std::cout << _name << ", form, created.\n";
}

AForm::AForm(std::string const &name, int gradeToSign, int gradeToExecute) : _name(name), _isSigned(false), _gradeToSign(gradeToSign), _gradeToExecute(gradeToExecute)
{
	if (gradeToSign > _gradeMax || gradeToExecute > _gradeMax)
		throw AForm::GradeTooLowException();
	if (gradeToSign < _gradeMin || gradeToExecute < _gradeMin)
		throw AForm::GradeTooHighException();

	std::cout << _name << ", form, created.\n";
}

AForm::AForm(AForm const &other) : _name(other._name), _isSigned(other._isSigned), _gradeToSign(other._gradeToSign), _gradeToExecute(other._gradeToExecute)
{
	std::cout << _name << ", form, copied.\n";
}

AForm &AForm::operator=(AForm const &other)
{
	if (this != &other)
		_isSigned = other._isSigned;
	std::cout << _name << ", form, assigned.\n";
	return (*this);
}

AForm::~AForm()
{
	std::cout << _name << ", form, destroyed.\n";
}

std::string const &AForm::getName() const { return _name; }

bool AForm::getIsSigned() const { return _isSigned; }
int AForm::getGradeToSign() const { return _gradeToSign; }
int AForm::getGradeToExecute() const { return _gradeToExecute; }

void AForm::beSigned(Bureaucrat const &bureaucrat)
{
	if (bureaucrat.getGrade() > _gradeToSign)
		throw AForm::GradeTooLowException();
	_isSigned = true;
}
//NEW
void AForm::execute(Bureaucrat const& executor) const
{
	if(!_isSigned)
		throw AForm::FormNotSignedException();
	if(executor.getGrade() > _gradeToExecute)
		throw AForm::GradeTooLowException();
	executeAction();
}

const char *AForm::GradeTooHighException::what() const throw()
{
	return "grade too high";
}

const char *AForm::GradeTooLowException::what() const throw()
{
	return "grade too low";
}
//NEW
const char *AForm::FormNotSignedException::what() const throw()
{
	return "form is not signed";
}

std::ostream &operator<<(std::ostream &os, AForm const &form)
{
	os << form.getName() << ", form, requires grade " << form.getGradeToSign() << " to sign and grade " << form.getGradeToExecute() << " to execute, "
	   << (form.getIsSigned() ? "signed" : "not signed") << ".\n";
	return os;
}