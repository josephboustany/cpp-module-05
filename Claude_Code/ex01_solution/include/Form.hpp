#ifndef FORM_HPP
#define FORM_HPP

#include <string>
#include <exception>
#include <ostream>

/*
** Same reasoning as in Bureaucrat.hpp: beSigned() only takes a Bureaucrat by
** reference, so a forward declaration is enough here. The full class
** definition is only needed in Form.cpp, where bureaucrat.getGrade() is
** actually called.
*/
class Bureaucrat;

class Form
{
	public:
		/*
		** Default constructor: uses hardcoded, already-known-valid values
		** (no caller input, so no need to re-run the grade validation that
		** the parameterized constructor below performs).
		*/
		Form();
		Form(std::string const& name, int gradeToSign, int gradeToExecute);
		Form(Form const& other);
		Form& operator=(Form const& other);
		~Form();

		std::string const& getName() const;
		bool getIsSigned() const;
		int getGradeToSign() const;
		int getGradeToExecute() const;

		/*
		** Marks this Form as signed, if bureaucrat's grade is high enough
		** (numerically <= _gradeToSign -- remember grade 1 outranks grade
		** 150). Throws Form::GradeTooLowException otherwise.
		*/
		void beSigned(Bureaucrat const& bureaucrat);

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
		** _name, _gradeToSign and _gradeToExecute are const: set once at
		** construction, never changed afterward. Because of this, the copy
		** assignment operator below can only copy _isSigned -- assigning to
		** a const member is a compile error.
		*/
		const std::string _name;
		bool _isSigned;
		const int _gradeToSign;
		const int _gradeToExecute;

		// Same pattern as Bureaucrat's _gradeMin/_gradeMax: declared here,
		// defined (with its value) in Form.cpp, so it always links cleanly
		// even if ever odr-used.
		static const int _gradeMin;
		static const int _gradeMax;
};

std::ostream& operator<<(std::ostream& out, Form const& form);

#endif
