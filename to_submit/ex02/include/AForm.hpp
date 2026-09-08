#ifndef AFORM_HPP
#define AFORM_HPP

#include <exception>
#include <string>
#include <ostream>

class Bureaucrat;

class AForm
{
public:
	AForm();
	AForm(std::string const &name, int gradeToSign, int gradeToExecute);
	AForm(AForm const &other);
	AForm &operator=(AForm const &other);
	virtual ~AForm();//New

	std::string const &getName() const;
	bool getIsSigned() const;
	int getGradeToSign() const;
	int getGradeToExecute() const;

	void beSigned(Bureaucrat const &bureaucrat);
	void execute(Bureaucrat const &Bureaucrat) const;//New

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
	//New
	class FormNotSignedException : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
	//Why protected and not private:Only execute() (inherited by every derived class)
	//is meant to call it
protected:
	virtual void executeAction() const = 0;

private:
	const std::string _name;
	bool _isSigned;
	const int _gradeToSign;
	const int _gradeToExecute;

	static const int _gradeMin;
	static const int _gradeMax;
};

std::ostream &operator<<(std::ostream &os, const AForm &AForm);

#endif