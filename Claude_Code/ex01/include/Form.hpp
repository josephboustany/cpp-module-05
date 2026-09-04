#ifndef FORM_HPP
#define FORM_HPP

#include <exception>
#include <string>
#include <ostream>

class Bureaucrat;

class Form
{
public:
	Form();
	Form(std::string const &name, int gradeToSign, int gradeToExecute);
	Form(Form const &other);
	Form &operator=(Form const &other);
	~Form();

	std::string const &getName() const;
	bool getIsSigned() const;
	int getGradeToSign() const;
	int getGradeToExecute() const;

	void beSigned(Bureaucrat const &bureaucrat);

	class GradeTooHighException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};

	class GradeTooLowException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};

private:
	const std::string _name;
	bool _isSigned;
	const int _gradeToSign;
	const int _gradeToExecute;

	// since if(gradeToSign < Bureaucrat::_gradeMax) won't compile — private
	static const int _gradeMin;
	static const int _gradeMax;
};

std::ostream &operator<<(std::ostream &os, const Form &form);

#endif