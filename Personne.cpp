#include "Personne.h"

Personne::Personne()
{

}
Personne::~Personne()
{

}
QString Personne::getNom()
{
    return (nom);
}
QString Personne::getPrenom()
{
    return (prenom);
}
int Personne::getAge()
{
    return (age);
}
void Personne::setNom(QString n)
{
    nom = n;
}
void Personne::setPrenom(QString p)
{
    prenom = p;
}
void Personne::setAge(int a)
{
    age = a;
}
bool Personne::operator==(Personne autre)
{
    bool rep;
    rep = ((nom == autre.getNom()) && (prenom == autre.getPrenom()) && (age == autre.getAge()));
    return (rep);
}