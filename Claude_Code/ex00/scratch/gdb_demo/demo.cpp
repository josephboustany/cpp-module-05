#include <iostream>

// ── Correct hierarchy: virtual dispatch ─────────────────────────────
class Animal
{
	public:
		virtual std::string makeSound() const { return "..."; }
		virtual ~Animal() {}
};

class Dog : public Animal
{
	public:
		std::string makeSound() const { return "Woof!"; }
};

class Cat : public Animal
{
	public:
		std::string makeSound() const { return "Meow!"; }
};

// ── Broken hierarchy: no virtual, static dispatch ───────────────────
class WrongAnimal
{
	public:
		std::string makeSound() const { return "..."; }
};

class WrongDog : public WrongAnimal
{
	public:
		std::string makeSound() const { return "Woof!"; }
};

int main()
{
	Animal* a = new Dog();
	std::cout << a->makeSound() << "\n";
	delete a;

	WrongAnimal* wa = new WrongDog();
	std::cout << wa->makeSound() << "\n";
	delete wa;

	return 0;
}
