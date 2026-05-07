#include "Inscription.h"
#include "ui_Inscription.h"
#include "Personne.h"
#include <QMessageBox>
#include <QTabWidget>
#include <QWidgetItem>
#include <QFileDialog>
#include <fstream>
#include <QCloseEvent>
#include <QMenu>
#include <QAction>


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
    int nLigne ;

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
            nLigne = tableau->rowCount();
            tableau->insertRow(nLigne);

            nom = new QTableWidgetItem();
            prenom = new QTableWidgetItem();
            age = new QTableWidgetItem();
            // insertion du nom
            nom->setText(iPers->getNom());
            tableau->setItem(nLigne , 0 , nom);
            // prenom
            prenom->setText(iPers->getPrenom());
            tableau->setItem(nLigne , 1 , prenom);
            // age vers QString pour la troisieme colonne
            age->setText(QString::number(iPers->getAge()));
            tableau->setItem(nLigne , 2 , age);
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
bool Inscription::estModifier()
{
    int i;
    // consideré comme modification s'il n'est plus comme l'etat initial

    if(historique.empty())
    {
        return (false);
    }

    if(std::size(liste) != std::size(historique.front()))
    {
        return (true);
    }

    for(i=0 ; i<liste.size() ; i++)
    {
        if(!(liste[i] == historique.front()[i]))
        {
            return (true);
        }
    }
    return (false);
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
            if((position > std::size(liste)) || (position < 0))
            {
                QMessageBox::warning(this , "ErReur d'ajout" , "la position choisit doit etre entre 1 et la taille du tableau !");
            }
            else
            {
                //etat actuelle
                historique.push_back(liste);
                histoRetablir.clear();

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
    int nLigne ;

    // definition des valeurs
    tableau = new QTableWidget();
    tableau->setColumnCount(3);
    tableau->setHorizontalHeaderLabels({"Nom", "Prenom", "Age"});
    pers = liste.begin();

    tableau->blockSignals(true);
    while(pers != liste.end())
    {
        nLigne = tableau->rowCount();
        tableau->insertRow(nLigne);

        // QTableWidget n'accepte que de nouvelle element à inserer
        nom = new QTableWidgetItem();
        prenom = new QTableWidgetItem();
        age = new QTableWidgetItem();

        nom->setText(pers->getNom());
        tableau->setItem(nLigne , 0 , nom);

        prenom->setText(pers->getPrenom());
        tableau->setItem(nLigne , 1 , prenom);

        age->setText(QString::number(pers->getAge()));
        tableau->setItem(nLigne , 2 , age);
        pers++;
    }

    tableau->blockSignals(false);
    connect(tableau , &QTableWidget::itemChanged , this , &Inscription::tableauEditer );
    // activation du menu contextuel
    tableau->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableau , &QWidget::customContextMenuRequested , this , &Inscription::menuContextuel);

    ui->scroll->setWidget(tableau);
    ui->scroll->setWindowTitle("Liste des inscrit");
    ui->scroll->show();
}
void Inscription::menuContextuel(const QPoint &position)
{
    QTableWidgetItem *element;
    QMenu menu;
    QAction *modifier , *supprimer , *choix;
    int nLigne;

    element = tableau->itemAt(position);
    // si la clic droit n'est pas dans une zone vide
    if(element)
    {
        nLigne = element->row();
        supprimer = menu.addAction("Supprimer");
        modifier = menu.addAction("modifier");
        choix = menu.exec(tableau->viewport()->mapToGlobal(position));

        if(choix == supprimer)
        {
            historique.push_back(liste);
            histoRetablir.clear();
            liste.erase(liste.begin() + nLigne);
            lister();
        }
        else if(choix == modifier)
        {
            ui->nom->setText(liste[nLigne].getNom());
            ui->prenom->setText(liste[nLigne].getPrenom());
            ui->age->setValue(liste[nLigne].getAge());
            ui->position->setValue(nLigne + 1);
            ui->checkBox->setChecked(true);

            // ajouter pour valider
            historique.push_back(liste);
            histoRetablir.clear();
            liste.erase(liste.begin() + nLigne);
            lister();
        }
    }
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

    nomFichier = QFileDialog::getSaveFileName(this , "Sauvegarder en fichier le feuille" , "" , "filtre csv (*.csv)");
    if(nomFichier.length() == 0)
    {
        QMessageBox::warning(this , "annulation" , "Aucun nom de fichier n'a été introduit !");
        return;
    }
    fichier = nomFichier.toStdString();
    sorti.open(fichier);

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
    QString nomFichier, nom, prenom , message;
    string fichier, chaine;
    vector<vector <Personne>> tmp;
    vector<string> champs;
    ifstream entree;
    int age , nLigne;
    Personne pers;

    nomFichier = QFileDialog::getOpenFileName(this , "Importez un fichier l'editeur" , "" , "filtre csv (*.csv)");
    if(nomFichier.length() == 0)
    {
        QMessageBox::warning(this , "annulation" , "Aucun fichier n'a été selectionné !");
        return;
    }
    if(!(nomFichier.isEmpty()))
    {
        fichier = nomFichier.toStdString();
        entree.open(fichier);

        if(entree)
        {
            // preserver en cas d'erreur du lecture de fichier actuelle
            historique.push_back(liste);
            tmp = historique;
            // pour voir les etats de modification
            historique.clear();
            liste.clear();

            nLigne = 1;
            while(getline(entree, chaine))
            {
                champs = separe(',', chaine);
                if(std::size(champs) == 3)
                {
                    nom = QString::fromStdString(champs.at(0));
                    prenom = QString::fromStdString(champs.at(1));
                    age = atoi(champs.at(2).c_str());
                    if((age == 0) || (!isalpha(nom.toStdString().at(0))) || (!isalpha(prenom.toStdString().at(0))))
                    {
                        message = "Erreur a la ligne : " + QString::number(nLigne);
                        QMessageBox::warning(this , "Erreur de nommage" , message);
                        // retourner à l'ancien liste en cas d'erreur
                        historique = tmp;
                        liste = historique.back();
                        historique.pop_back();
                    }
                    pers.setNom(nom);
                    pers.setPrenom(prenom);
                    pers.setAge(age);
                    liste.push_back(pers);
                }
                else
                {
                    message = "Ce fichier presente une erreur de formatage à la ligne " + QString::number(nLigne);
                    message += ".\n Arret de la lecture !";
                    QMessageBox::warning(this , "ERREUR" , message);
                    // retourner à l'ancien liste en cas d'erreur
                    historique = tmp;
                    liste = historique.back();
                    historique.pop_back();
                }
                nLigne++;
            }
            entree.close();
            lister();
            histoRetablir.clear();
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
    bool modifie;

    modifie = estModifier();
    if(modifie)
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
    else
    {
        event->accept();
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
