#ifndef PERSONNE_H
#define PERSONNE_H

#include <QString>
using namespace  std;

class Personne
{
    public:
        Personne();
        ~Personne();
        QString getNom();
        QString getPrenom();
        int getAge();
        void setNom(QString n);
        void setPrenom(QString p);
        void setAge(int a);
        bool operator==(Personne autre);
    private:
        QString nom;
        QString prenom;
        int age;

};

#endif
