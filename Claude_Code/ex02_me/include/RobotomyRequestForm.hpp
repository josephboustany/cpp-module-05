#ifndef ROBOTOMYREQUESTFORM_HPP
#define ROBOTOMYREQUESTFORM_HPP

#include "AForm.hpp"

class RobotomyRequetForm : public AForm
{
public:
	RobotomyRequetForm();
	RobotomyRequetForm(std::string const &target);
	RobotomyRequetForm(RobotomyRequetForm const& other);
	RobotomyRequetForm &operator=(RobotomyRequetForm const &other);
	~RobotomyRequetForm();


protected:
	virtual void executeAction() const;
private:
	const std::string _target;
};

#endif