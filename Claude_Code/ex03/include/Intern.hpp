#ifndef INTERN_HPP
#define INTERN_HPP

#include <string>

class AForm;

class Intern
{
public:
	Intern();
	Intern(Intern const &other);
	Intern &operator=(Intern const &other);
	~Intern();

	AForm *makeForm(std::string const &formName, std::string const &target) const;

private:
	static AForm *createShrubberyCreationForm(std::string const &target);
	static AForm *createRobotomyRequestForm(std::string const &target);
	static AForm *createPresidentialPardonForm(std::string const &target);

	static const std::string _formNames[3];
	static AForm *(*_formCreators[3])(std::string const &target);
};

#endif
