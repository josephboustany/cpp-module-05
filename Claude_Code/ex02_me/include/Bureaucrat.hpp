#ifndef BUREAUCRAT_HPP
#define BUREAUCRAT_HPP

#include <string>
#include <exception>
#include <ostream>

class AForm;

class Bureaucrat
{
	public:

		Bureaucrat();
		Bureaucrat(std::string const& name, int grade);
		Bureaucrat(Bureaucrat const& other);
		Bureaucrat& operator=(Bureaucrat const& other);
		~Bureaucrat();

		std::string const& getName() const;
		int getGrade() const;

		void incrementGrade();
		void decrementGrade();
		void signForm(AForm& form) const;
		void executeForm(AForm const& form) const;

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
		int _grade;

		static const int _gradeMin;
		static const int _gradeMax;
};

std::ostream& operator<<(std::ostream& out, Bureaucrat const& bureaucrat);

#endif
