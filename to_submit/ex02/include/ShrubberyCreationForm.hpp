#ifndef SHRUBBERYCREATIONFORM_HPP
#define SHRUBBERYCREATIONFORM_HPP

#include "AForm.hpp"


class ShrubberyCreationForm : public AForm
{
public:
	ShrubberyCreationForm();
	ShrubberyCreationForm(std::string const& target);
	ShrubberyCreationForm(ShrubberyCreationForm const& other);
	ShrubberyCreationForm &operator=(ShrubberyCreationForm const& other);
	~ShrubberyCreationForm();

//The class's own members plus any derived class. Invisible to everyone else
protected:
	virtual void executeAction() const;


//private — only the class's own member functions.
private:
	const std::string _target;
};

#endif