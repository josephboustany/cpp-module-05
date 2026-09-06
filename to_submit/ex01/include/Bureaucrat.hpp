#ifndef BUREAUCRAT_HPP
#define BUREAUCRAT_HPP

#include <string>
#include <exception>
#include <ostream>

class Form;

class Bureaucrat
{
	public:

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
		void signForm(Form& form) const;

		//public in ": public std.." is not about who can access the class.
		//It's about what happens to the base class's members inside the derived class.
		//we must  use here public.otherwise 2 things break:e.what() in main.cpp would not compile because
		//With private inheritance, what() becomes private in your class — outside code can't call it.
		//The only practical consequence of nesting is the qualification requirement:
		//-Inside Bureaucrat's own member functions (constructor, incrementGrade()...)→ short name works: throw GradeTooHighException();
		//- Anywhere outside the class (in main(), in a catch block, in another class) → you must write the full path: Bureaucrat::GradeTooHighException
		class GradeTooHighException : public std::exception
		{
			public:
				virtual const char* what() const throw();
				//when we say throw(), we mean This function doesn't call new,
				//doesn't construct a std::string, doesn't do anything that 
				//could fail and raise an exception. So it can honestly promise 
				//throw() — "I guarantee I will never throw," full stop.
				//It's not accepting or rejecting anything; it just never has a reason to throw in the first place.
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
