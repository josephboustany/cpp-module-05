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
	// virtual: AForm is a polymorphic base (it has a pure virtual member
	// below), and objects of its derived classes may be deleted through
	// an AForm pointer/reference. Without virtual here, only ~AForm()
	// would run on delete, skipping the derived destructor.
	virtual ~AForm();

	std::string const &getName() const;
	bool getIsSigned() const;
	int getGradeToSign() const;
	int getGradeToExecute() const;

	void beSigned(Bureaucrat const &bureaucrat);

	// Not pure: execute() is the shared skeleton (check signed, check
	// grade, then hand off to executeAction()). It is identical for every
	// concrete form, so it lives here instead of being copy-pasted into
	// each one.
	void execute(Bureaucrat const &executor) const;

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

	class FormNotSignedException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};

protected:
	// The one pure virtual member: every concrete form must define what
	// "doing its job" actually means. This is the only thing that makes
	// AForm abstract -- everything else above is a normal, fully-defined
	// member function shared by all of them. Protected, not public: only
	// execute() (inherited by every derived class) is meant to call it.
	virtual void executeAction() const = 0;

private:
	const std::string _name;
	bool _isSigned;
	const int _gradeToSign;
	const int _gradeToExecute;

	// since if(gradeToSign < Bureaucrat::_gradeMax) won't compile — private
	static const int _gradeMin;
	static const int _gradeMax;
};

std::ostream &operator<<(std::ostream &os, const AForm &form);

#endif