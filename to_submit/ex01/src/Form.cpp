#include <iostream>
#include "Form.hpp"
#include "Bureaucrat.hpp"

const int Form::_gradeMin = 1;
const int Form::_gradeMax = 150;

Form::Form() : _name(""), _isSigned(false), _gradeToSign(_gradeMax), _gradeToExecute(_gradeMax)
{
	std::cout << _name << ", form, created.\n";
}

Form::Form(std::string const &name, int gradeToSign, int gradeToExecute) : _name(name), _isSigned(false), _gradeToSign(gradeToSign), _gradeToExecute(gradeToExecute)
{
	if (gradeToSign > _gradeMax || gradeToExecute > _gradeMax)
		throw Form::GradeTooLowException();
	if (gradeToSign < _gradeMin || gradeToExecute < _gradeMin)
		throw Form::GradeTooHighException();

	std::cout << _name << ", form, created.\n";
}

Form::Form(Form const &other) : _name(other._name), _isSigned(other._isSigned), _gradeToSign(other._gradeToSign), _gradeToExecute(other._gradeToExecute)
{
	std::cout << _name << ", form, copied.\n";
}

Form &Form::operator=(Form const &other)
{
	if (this != &other)
		_isSigned = other._isSigned;
	std::cout << _name << ", form, assigned.\n";
	return (*this);
}

Form::~Form()
{
	std::cout << _name << ", form, destroyed.\n";
}

std::string const &Form::getName() const { return _name; }

bool Form::getIsSigned() const { return _isSigned; }
int Form::getGradeToSign() const { return _gradeToSign; }
int Form::getGradeToExecute() const { return _gradeToExecute; }

void Form::beSigned(Bureaucrat const &bureaucrat)
{
	if (bureaucrat.getGrade() > _gradeToSign)
		throw Form::GradeTooLowException();
	_isSigned = true;
}

const char *Form::GradeTooHighException::what() const throw()
{
	return "grade too high";
}

const char *Form::GradeTooLowException::what() const throw()
{
	return "grade too low";
}

std::ostream &operator<<(std::ostream &os, Form const &form)
{
	os << form.getName() << ", form, requires grade " << form.getGradeToSign() << " to sign and grade " << form.getGradeToExecute() << " to execute, "
	   << (form.getIsSigned() ? "signed" : "not signed") << ".\n";
	return os;
}