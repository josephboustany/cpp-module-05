#ifndef FORM_HPP
#define	FORM_HPP

#include<string>
#include<exception>
#include<ostream>

class Bureaucrat;

class Form
{

	public:

		Form();
		Form(std::string const& name, int const gradeSign, int const gradeExec);
		Form(Form const& other);
		Form& operator=(Form const& other);
		~Form();

		std::string getName() const;
		const int getGradeSign() const;
		const int getGradeExec() const;

		class GradeTooHighException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		class GradeTooLowException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

	private:
		const std::string _name;
		bool _isSigned;
		const int _gradeSign;
		const int _gradeExec;

};

std::ostream& operator<<(std::ostream& os, const Form& form);

#endif