#include <iostream>
#include "Bureaucrat.hpp"

const int Bureaucrat::_gradeMin = 1;
const int Bureaucrat::_gradeMax = 150;

Bureaucrat::Bureaucrat() : _name(""), _grade(_gradeMax)
{
	std::cout << _name << ", bureaucrat grade " << _grade << ", created.\n";
}

Bureaucrat::Bureaucrat(std::string const& name, int grade) : _name(name), _grade(grade)
{
	/*
	** _grade is set through the member initializer list above, then
	** validated here. If it is out of range the object is never handed
	** back to the caller: the exception unwinds the stack and _name
	** (already fully constructed) is destroyed normally.
	*/
	if (grade < _gradeMin)
		throw Bureaucrat::GradeTooHighException();
	if (grade > _gradeMax)
		throw Bureaucrat::GradeTooLowException();
	std::cout << _name << ", bureaucrat grade " << _grade << ", created.\n";
}

Bureaucrat::Bureaucrat(Bureaucrat const& other) : _name(other._name), _grade(other._grade)
{
	std::cout << _name << ", bureaucrat grade " << _grade << ", copied.\n";
}

Bureaucrat& Bureaucrat::operator=(Bureaucrat const& other)
{
	if (this != &other)
	{
		/*
		** _name is const and cannot be reassigned; only _grade is copied.
		** other._grade has already passed the range check in its own
		** constructor, so no re-validation is needed here.
		*/
		_grade = other._grade;
	}
	std::cout << _name << ", bureaucrat grade " << _grade << ", assigned.\n";
	return *this;
}

Bureaucrat::~Bureaucrat()
{
	std::cout << _name << ", bureaucrat grade " << _grade << ", destroyed.\n";
}

std::string const& Bureaucrat::getName() const
{
	return _name;
}

int Bureaucrat::getGrade() const
{
	return _grade;
}

void Bureaucrat::incrementGrade()
{
	if (_grade - 1 < _gradeMin)
		throw Bureaucrat::GradeTooHighException();
	_grade--;
}

void Bureaucrat::decrementGrade()
{
	if (_grade + 1 > _gradeMax)
		throw Bureaucrat::GradeTooLowException();
	_grade++;
}

const char* Bureaucrat::GradeTooHighException::what() const throw()
{
	return "Bureaucrat grade too high (must be 1 or higher)";
}

const char* Bureaucrat::GradeTooLowException::what() const throw()
{
	return "Bureaucrat grade too low (must be 150 or lower)";
}

std::ostream& operator<<(std::ostream& out, Bureaucrat const& bureaucrat)
{
	out << bureaucrat.getName() << ", bureaucrat grade " << bureaucrat.getGrade()<< ".";
	return out;
}
