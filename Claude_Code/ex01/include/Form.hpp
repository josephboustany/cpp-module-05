#ifndef FORM_HPP
#define	FORM_HPP

#include <exception>
#include <string>
#include <ostream>

class Bureaucrat;

class Form
{
	public:

	Form();
	Form(int gradeToSign, int gradeToExec, std::string const& name);
	Form(Form const& other);
	Form& operator=(Form const& other);
	~Form();

	std::string const& getName() const;
	int getGradeToSign() const;
	int getGradeToExecute() const;
	bool getIsSigned() const;

	void beSigned(Bureaucrat const& bureaucrat);

	class GradeTooLowException : public std::exception
	{
		public:
			virtual const char* what() const throw(); 

	};

		class GradeTooHighException : public std::exception
	{
		public:
			virtual const char* what() const throw(); 

	};

	private:

	const std::string _name;
	bool _isSigned;
	const int _gradeToSign;
	const int _gradeToExecute;

};

std::ostream& operator<<(std::ostream& os, const Form& form);


#endif