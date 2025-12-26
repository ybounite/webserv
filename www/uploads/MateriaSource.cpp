#include "MateriaSource.hpp"

// constructor
MateriaSource::MateriaSource()
{
    for (int i = 0; i < 4; ++i)
        this->_materias[i] = NULL;
}

// copy constructor
MateriaSource::MateriaSource(const MateriaSource &other) { *this = other; }

// copy assignment
MateriaSource &MateriaSource::operator=(const MateriaSource &other)
{
    for (int i = 0; i < 4; i++)
    {
        this->_materias[i] = other._materias[i];
    }
    return *this;
}

// add the materia to the _materia array
void MateriaSource::learnMateria(AMateria *materia)
{
    for (int i = 0; i < 4; i++)
    {
        if (this->_materias[i] == NULL)
        {
            this->_materias[i] = materia->clone();
            delete materia;
            break;
        }
    }
}

// look whether the materia is existed and return a copy of it
AMateria *MateriaSource::createMateria(std::string const &materia)
{
    for (int i = 0; i < 4; i++)
    {
        if (this->_materias[i])
        {
            if (this->_materias[i]->getType() == materia)
            {
                return this->_materias[i]->clone();
            }
        }
    }
    return (0);
}

// destructor
MateriaSource::~MateriaSource()
{
    for (int i = 0; i < 4; i++)
    {
        if (this->_materias[i] != NULL)
            delete this->_materias[i];
    }
}
