#ifndef BUREAUCRAT_HPP
#define BUREAUCRAT_HPP

#include <string>
#include <exception>
#include <ostream>

/*
** Form.cpp needs a complete Bureaucrat (it calls bureaucrat.getGrade()
** inside beSigned()), so Form.cpp includes Bureaucrat.hpp fully.
** This header only needs to know that "Form" is a type -- signForm() takes
** it by reference, and a reference to an incomplete type is fine as long as
** nothing here dereferences it. So a forward declaration is enough here,
** breaking the circular #include that would otherwise happen if both
** headers tried to fully include each other.
*/
class Form;

class Bureaucrat
{
	public:
		/*
		** Default constructor: uses hardcoded, already-known-valid values
		** (no caller input, so no need to re-run the grade validation that
		** the parameterized constructor below performs).
		*/
		Bureaucrat();
		Bureaucrat(std::string const& name, int grade);
		Bureaucrat(Bureaucrat const& other);
		Bureaucrat& operator=(Bureaucrat const& other);
		~Bureaucrat();

		//std::string alone would copy by value in each call
		//we can hand back a reference to it without copying
		//The const on & stops the caller from modifying our private member
		//through that reference: bureaucrat.getName() = "hack"
		std::string const& getName() const;
		int getGrade() const;

		void incrementGrade();
		void decrementGrade();

		/*
		** Attempts to have this Bureaucrat sign the given Form. Takes the
		** Form by reference (not by pointer): the caller keeps ownership,
		** this function only borrows it to call its beSigned() method.
		*/
		void signForm(Form& form);

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
		/*
		** _name is const: it is set once at construction and never changes.
		** Because of this, the copy assignment operator below can only
		** copy _grade -- assigning to a const member is a compile error.
		*/
		const std::string _name;
		int _grade;

		// Declared here, defined (with its value) in Bureaucrat.cpp.
		// The in-class form alone (static const int _gradeMin = 1;) is only a value,
		// not a real object -- if this member is ever odr-used (address taken, bound
		// to a reference, etc.) without an out-of-class definition, it links with
		// "undefined reference". Defining it in the .cpp always avoids that trap.
		static const int _gradeMin;
		static const int _gradeMax;
};

std::ostream& operator<<(std::ostream& out, Bureaucrat const& bureaucrat);

#endif
