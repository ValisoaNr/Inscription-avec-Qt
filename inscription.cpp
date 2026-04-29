#include "inscription.h"
#include "ui_inscription.h"
#include "Personne.h"
#include <QMessageBox>
#include <QTabWidget>
#include <QWidgetItem>
#include <QFileDialog>
#include <fstream>
#include <QCloseEvent>


Inscription::Inscription(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Inscription)
{
    ui->setupUi(this);
    connect(ui->ajouter , SIGNAL(clicked()) , this , SLOT(ajouter()));
    connect(ui->rechercher , SIGNAL(clicked()) , this , SLOT(rechercher()));
    connect(ui->supprimer , SIGNAL(clicked()) , this , SLOT(supprimer()));
    connect(ui->lister , SIGNAL(clicked()) , this , SLOT(lister()));
    connect(ui->actionAnnuler , SIGNAL(triggered()) , this , SLOT(Annuler()));
    connect(ui->actionOuvrir , SIGNAL(triggered()) , this , SLOT(OuvrirFichier()));
    connect(ui->actionRetablir , SIGNAL(triggered()) , this , SLOT(Retablir()));
    connect(ui->actionEnregistrer , SIGNAL(triggered()) , this , SLOT(EnregistrerFichier()));

    historique.push_back(liste);
}
vector<Personne> Inscription::getListe()
{
    return (liste);
}
vector<Personne> Inscription::trouve(Personne individu , vector<Personne> listeP)
{
    vector<Personne> resultat;
    vector<Personne>::iterator iPers;

    iPers = listeP.begin();
    while(iPers != listeP.end())
    {
        if((iPers->getNom() == individu.getNom()) || (iPers->getPrenom() == individu.getPrenom()) || (iPers->getAge() == individu.getAge()))
        {
            resultat.insert(resultat.end() , *iPers);
        }
        iPers++;
    }

    return (resultat);
}
void Inscription::rechercher()
{
    Personne pers;
    vector<Personne> recherche;
    vector<Personne>::iterator iPers;
    QTableWidget *tableau;
    QTableWidgetItem *nom , *prenom , *age;
    int row ;

    initialise(pers , true);
    recherche = trouve(pers , liste);
    if(std::size(recherche) != 0)
    {
        // definition des valeurs
        tableau = new QTableWidget();
        tableau->setColumnCount(3);
        tableau->setHorizontalHeaderLabels({"Nom", "Prenom", "Age"});
        iPers = recherche.begin();
        while(iPers != recherche.end())
        {
            row = tableau->rowCount();
            tableau->insertRow(row);

            nom = new QTableWidgetItem();
            prenom = new QTableWidgetItem();
            age = new QTableWidgetItem();
            // insertion du nom
            nom->setText(iPers->getNom());
            tableau->setItem(row , 0 , nom);
            // prenom
            prenom->setText(iPers->getPrenom());
            tableau->setItem(row , 1 , prenom);
            // age vers QString pour la troisieme colonne
            age->setText(QString::number(iPers->getAge()));
            tableau->setItem(row , 2 , age);
            iPers++;
        }

        ui->scroll->setWidget(tableau);
        ui->scroll->setWindowTitle("Recherche");
        ui->scroll->show();
    }
    else
    {
        QMessageBox::information(this , "Introuvable" , "Veuillez bien remplir les champs pour une bonne recherche !");
    }
}
void Inscription::setListe(vector<Personne> l)
{
    liste = l;
}
int Inscription::initialise(Personne& individu , bool accepteErreur=false)
{
    QChar caractere;
    int valide;    // nombre de champ valide

    valide = 0;
    // nom
    if((ui->nom->text().length() == 0)  && ( ! accepteErreur))
    {
        QMessageBox::warning(this , "Erreur" , "nom ne peut pas etre vide !");
    }
    else
    {
        if(ui->nom->text().length() != 0)
        {
            caractere = ui->nom->text().at(0);
            if((caractere.isDigit()) && ( ! accepteErreur))
            {
                QMessageBox::warning(this , "Erreur" , " le nom ne doit pas commencer par un nombre");
            }
            else
            {
                individu.setNom(ui->nom->text());
                valide++;
            }
        }
    }
    // prenom
    if((ui->prenom->text().length() == 0)  && ( ! accepteErreur))
    {
        QMessageBox::warning(this , "Erreur" , "prenom ne peut pas etre vide !");
    }
    else
    {
        if(ui->prenom->text().length() != 0)
        {
            caractere = ui->prenom->text().at(0);
            if((caractere.isDigit())  && ( ! accepteErreur))
            {
                QMessageBox::warning(this , "Erreur" , " le prenom ne doit pas commencer par un nombre");
            }
            else
            {
                caractere = ui->prenom->text().at(0);
                individu.setPrenom(ui->prenom->text());
                valide++;
            }
        }
    }
    // age
    if((ui->age->value() == 0) && ( ! accepteErreur))
    {
        QMessageBox::warning(this , "Erreur" , "Age invalide !");
    }
    else
    {
        individu.setAge(ui->age->value());
        valide++;
    }
    return (valide);
}
void Inscription::ajouter()
{
    Personne pers;
    vector<Personne>::iterator iPers;
    int position , validite;

    validite = initialise(pers);
    if(validite == 3)
    {
        // s'il choisit une position
        if(ui->checkBox->isChecked())
        {
            // position moins un pour que l'indice 0 est le premier position
            position = ui->position->value() - 1;
        }
        else
        {
            position = std::size(liste);
        }
        // Eviter dans les cas ou la personne existe deja
        iPers = std::find(liste.begin() , liste.end() , pers);
        if(iPers != liste.end())
        {
            QMessageBox::warning(this , "ErReur d'ajout" , "C'est deja existant !");
        }
        else
        {
            //etat actuelle
            historique.push_back(liste);
            histoRetablir.clear();

            if((position >= std::size(liste)) || position < 0)
            {
                liste.push_back(pers);
            }
            else
            {
                iPers = liste.begin() + position ;
                liste.insert(iPers , pers);
            }
            lister();
        }
    }
}
void Inscription::lister()
{
    vector<Personne>::iterator pers;
    QTableWidgetItem *nom , *prenom , *age;
    int row ;

    // definition des valeurs
    tableau = new QTableWidget();
    tableau->setColumnCount(3);
    tableau->setHorizontalHeaderLabels({"Nom", "Prenom", "Age"});
    pers = liste.begin();

    tableau->blockSignals(true);
    while(pers != liste.end())
    {
        row = tableau->rowCount();
        tableau->insertRow(row);

        // QTableWidget n'accepte que de nouvelle element à inserer
        nom = new QTableWidgetItem();
        prenom = new QTableWidgetItem();
        age = new QTableWidgetItem();

        nom->setText(pers->getNom());
        tableau->setItem(row , 0 , nom);

        prenom->setText(pers->getPrenom());
        tableau->setItem(row , 1 , prenom);

        age->setText(QString::number(pers->getAge()));
        tableau->setItem(row , 2 , age);
        pers++;
    }

    tableau->blockSignals(false);
    connect(tableau , &QTableWidget::itemChanged , this , &Inscription::tableauEditer );

    ui->scroll->setWidget(tableau);
    ui->scroll->setWindowTitle("Liste des inscrit");
    ui->scroll->show();
}
void Inscription::tableauEditer(QTableWidgetItem *element)
{
    int ligne , colonne , age;
    bool ok;
    QString valeur;

    ligne = element->row();
    colonne = element->column();

    // Vérifier que la ligne correspond à un élément existant dans la liste
    if((ligne >= 0) && (ligne < (int)liste.size()))
    {
        historique.push_back(liste);
        histoRetablir.clear();

        valeur = element->text().trimmed();
        switch(colonne)
        {
            case 0:
            {
                liste[ligne].setNom(valeur);
                break;
            }
            case 1:
            {
                liste[ligne].setPrenom(valeur);
                break;
            }
            case 2:
            {
                age = valeur.toInt(&ok);
                if (ok && (age > 0))
                {
                    liste[ligne].setAge(age);
                }
                else
                {
                    // Valeur invalide : remettre l'ancienne valeur dans le tableau
                    tableau->blockSignals(true);
                    valeur = QString::number(liste[ligne].getAge());
                    element->setText(valeur);
                    tableau->blockSignals(false);
                }
                break;
            }
        }
    }

}
void Inscription::supprimer()
{
    Personne pers;
    vector<Personne>::iterator iPers;

    initialise(pers);
    iPers = std::find(liste.begin() , liste.end() , pers);
    if(iPers != liste.end())
    {
        historique.push_back(liste);
        histoRetablir.clear();
        liste.erase(iPers);
    }
    else
    {
        QMessageBox::warning(this , "Introuvable" , "Aucune personne correspondant !");
    }
    lister();
}
Inscription::~Inscription()
{
    delete ui;
    historique.clear();
    histoRetablir.clear();
    delete tableau;
}

void Inscription::Annuler()
{
    if(historique.size() > 1)
    {
        histoRetablir.push_back(liste);
        liste = historique.back();
        historique.pop_back();
        lister();
    }
}


void Inscription::Retablir()
{
    if(histoRetablir.size() != 0)
    {
        // Restaurer l'état depuis l'historique de rétablissement
        historique.push_back(liste);
        liste = histoRetablir.back();
        histoRetablir.pop_back();

        lister();
    }
}


void Inscription::EnregistrerFichier()
{
    QString nomFichier , qchaine;
    string fichier , chaine;
    ofstream sorti;
    vector<Personne>::iterator iPers;

    nomFichier = QFileDialog::getSaveFileName(this);
    fichier = nomFichier.toStdString();
    sorti.open(fichier.c_str());

    if(sorti)
    {
        iPers = liste.begin();
        while(iPers != liste.end())
        {
            qchaine = iPers->getNom() + "," + iPers->getPrenom() + "," + QString::number(iPers->getAge());
            chaine = qchaine.toStdString();
            sorti << chaine << endl;
            iPers++;
        }
        sorti.close();
        QMessageBox::information(this, "Succès", "Liste sauvegardée avec succès !");
    }
    else
    {
        QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir le fichier pour l'écriture !");
    }

}
void Inscription::OuvrirFichier()
{
    QString nomFichier, nom, prenom;
    string fichier, chaine;
    vector<string> champs;
    ifstream entree;
    int age;
    Personne pers;

    nomFichier = QFileDialog::getOpenFileName(this);
    if(!(nomFichier.isEmpty()))
    {
        fichier = nomFichier.toStdString();
        entree.open(fichier.c_str(), ios::in);

        if(entree)
        {
            historique.push_back(liste);
            liste.clear();

            while(getline(entree, chaine))
            {
                champs = separe(',', chaine);
                if(std::size(champs) == 3)
                {
                    nom = QString::fromStdString(champs.at(0));
                    prenom = QString::fromStdString(champs.at(1));
                    age = atoi(champs.at(2).c_str());

                    pers.setNom(nom);
                    pers.setPrenom(prenom);
                    pers.setAge(age);
                    liste.push_back(pers);
                }
            }
            entree.close();
            lister();
        }
        else
        {
            QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir le fichier !");
        }
    }
}
void Inscription::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reponse ;

    if(liste.empty())
    {
        event->accept();
    }
    else
    {
        reponse = QMessageBox::question(this , "Quitter" , "Voulez-vous enregistrer avant de quitter ?" , QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
        if(reponse == QMessageBox::Yes)
        {
            EnregistrerFichier();
            event->accept();
        }
        else if(reponse == QMessageBox::No)
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
}
vector<string> Inscription::separe(char sep, string chaine)
{
    vector<string> resultat;
    string tmp;
    int i = 0;

    while(i < chaine.length())
    {
        if(chaine[i] != sep)
        {
            tmp += chaine[i];
        }
        else
        {
            resultat.push_back(tmp);
            tmp = "";
        }
        i++;
    }
    resultat.push_back(tmp);
    return resultat;
}