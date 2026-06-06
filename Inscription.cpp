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
#include <QProgressBar>

Inscription::Inscription(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Inscription)
{
    ui->setupUi(this);

    // nom du fichier correspondant
    nFichier = "sans_titre.csv";
    modifier = false;
    // bar des status
    rechBoutton = new QPushButton();
    rechBoutton->setText("rechercher");
    rechEdit = new QLineEdit();
    ui->statusBar->addPermanentWidget(rechEdit);
    ui->statusBar->addPermanentWidget(rechBoutton);
    ui->statusBar->addPermanentWidget(new QLabel("[ Inscription avec Qt(en C++) ]"));
    connect(rechBoutton , &QPushButton::clicked , this , &Inscription::faireRecherche);
    connect(rechEdit , &QLineEdit::returnPressed , this , &Inscription::faireRecherche);

    connect(ui->ajouter , SIGNAL(clicked()) , this , SLOT(ajouter()));
    connect(ui->rechercher , SIGNAL(clicked()) , this , SLOT(rechercher()));
    connect(ui->supprimer , SIGNAL(clicked()) , this , SLOT(supprimer()));
    connect(ui->lister , SIGNAL(clicked()) , this , SLOT(lister()));
    connect(ui->actionAnnuler , SIGNAL(triggered()) , this , SLOT(annuler()));
    connect(ui->actionOuvrir , SIGNAL(triggered()) , this , SLOT(ouvrirFichier()));
    connect(ui->actionRetablir , SIGNAL(triggered()) , this , SLOT(retablir()));
    connect(ui->actionEnregistrer , SIGNAL(triggered()) , this , SLOT(enregistrerFichier()));
    // click entree pour prochain champ ou ajouter
    connect(ui->nom , SIGNAL(returnPressed()) , this , SLOT(deplaceCurseur()));
    connect(ui->prenom , SIGNAL(returnPressed()) , this , SLOT(deplaceCurseur()));
    connect(ui->age , SIGNAL(returnPressed()) , this , SLOT(deplaceCurseur()));

    // mes bar d'outils
    connect(ui->actionsupprSLC , SIGNAL(triggered()) , this , SLOT(supprSelection()));
    connect(ui->actionmodifierSLC , SIGNAL(triggered()) , this , SLOT(modifSelection()));

    // Desactiver en premier temps les boutons annuler et retablir
    ui->actionAnnuler->setEnabled(false);
    ui->actionRetablir->setEnabled(false);
    ui->actionmodifierSLC->setEnabled(false);
    ui->actionsupprSLC->setEnabled(false);

}
void Inscription::modifSelection()
{
    int ligne;

    ligne = getElementClic()[0];

    // Vérifier que la ligne correspond à un élément existant dans la liste
    if((ligne >= 0) && (ligne < (int)liste.size()))
    {
        ui->nom->setText(liste[ligne].getNom());
        ui->prenom->setText(liste[ligne].getPrenom());
        ui->age->setValue(liste[ligne].getAge());
        ui->position->setValue(ligne+ 1);
        ui->checkBox->setChecked(true);

        // ajouter pour valider
        sauveHisto();
        liste.erase(liste.begin() + ligne);
        lister();
    }

    ui->actionmodifierSLC->setEnabled(false);
    ui->actionsupprSLC->setEnabled(false);
    ui->nom->setFocus();
}
void Inscription::rechercher(Personne pers)
{
    vector<Personne> recherche;
    vector<Personne>::iterator iPers;
    QTableWidgetItem *nom , *prenom , *age;
    QLabel *message;
    int nLigne , i;

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
        tableau->setGeometry(0 , 0 , ui->scroll->width() , ui->scroll->height());
        ui->scroll->setWindowTitle("Recherche");
        ui->scroll->show();
        for(i=0 ; i<3 ; i++)
        {
            tableau->setColumnWidth(i , tableau->width()/3 );
        }
    }
    else
    {
        QMessageBox::information(this , "Introuvable" , "Aucune Personne correspondante ! \n\n Veuillez bien remplir le champs pour une bonne recherche !");
        if(tableau != nullptr)
        {
            tableau->clear();
            message = new QLabel("Aucun resultat !");
            ui->scroll->setWidget(message);
        }
    }
}
void Inscription::supprSelection()
{
    int ligne;

    ligne = getElementClic()[0];

    // Vérifier que la ligne correspond à un élément existant dans la liste
    if((ligne >= 0) && (ligne < (int)liste.size()))
    {
        sauveHisto();
        liste.erase(liste.begin() + ligne);
        messageStatBar((liste.begin() + ligne)->getNom() + " : Personne supprimé ");
        lister();
    }

    ui->actionsupprSLC->setEnabled(false);
    ui->actionmodifierSLC->setEnabled(false);
}
void Inscription::faireRecherche()
{
    Personne pers;
    bool convertible;

    pers.setNom(rechEdit->text());
    pers.setPrenom(rechEdit->text());
    rechEdit->text().toInt(&convertible);
    if(convertible)
    {
        pers.setAge(rechEdit->text().toInt());
    }

    rechercher(pers);

}
vector<Personne> Inscription::getListe()
{
    return (liste);
}
string Inscription::getNFichier()
{
    return (nFichier);
}
void Inscription::setNFichier(string nomFic)
{
    nFichier = nomFic;
}
void Inscription::deplaceCurseur()
{
    if(ui->nom->hasFocus())
    {
        if((ui->nom->text().length() == 0) || (ui->nom->text()[0].isDigit()) )
        {
            QMessageBox::warning(this , "Attention" , "Nom ne doit pas etre vide ou commencer par un nombre !");
        }
        else
        {
            ui->prenom->setFocus();
        }
    }
    else if(ui->prenom->hasFocus())
    {
        if((ui->prenom->text().length() == 0) || (ui->prenom->text()[0].isDigit()) )
        {
            QMessageBox::warning(this , "Attention" , "Prenom ne doit pas etre vide ou commencer par un nombre !");
        }
        else
        {
            ui->age->setFocus();
        }
    }
    else if(ui->age->hasFocus())
    {
        if(ui->age->value() == 0)
        {
            QMessageBox::warning(this , "Attention" , "Age doit pas etre egale 0 !");
        }
        else
        {
            ajouter();
            ui->scroll->setFocus();
        }
    }
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

    initialise(pers , true);
    rechercher(pers);
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
        ui->nom->setFocus();
    }
    else
    {
        if(ui->nom->text().length() != 0)
        {
            caractere = ui->nom->text().at(0);
            if((caractere.isDigit()) && ( ! accepteErreur))
            {
                QMessageBox::warning(this , "Erreur" , " le nom ne doit pas commencer par un nombre");
                ui->nom->setFocus();
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
        ui->prenom->setFocus();
    }
    else
    {
        if(ui->prenom->text().length() != 0)
        {
            caractere = ui->prenom->text().at(0);
            if((caractere.isDigit())  && ( ! accepteErreur))
            {
                QMessageBox::warning(this , "Erreur" , " le prenom ne doit pas commencer par un nombre");
                ui->prenom->setFocus();
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
        ui->age->setFocus();
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
                // Etat actuelle
                sauveHisto();

                iPers = liste.begin() + position ;
                liste.insert(iPers , pers);
                messageStatBar(pers.getNom() + " : personne ajouté");
            }
            lister();
        }
    }
    modifier = estModifier();
}
void Inscription::lister()
{
    vector<Personne>::iterator pers;
    QTableWidgetItem *nom , *prenom , *age;
    int nLigne  , i;

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

    // Pour les évènement de selection dans le tableau
    connect(tableau , &QTableWidget::cellClicked , this , &Inscription::elementCliquee);

    ui->scroll->setWidget(tableau);
    for(i=0 ; i<3 ; i++)
    {
        tableau->setColumnWidth(i , tableau->width()/3 );
    }

    ui->scroll->setWindowTitle("Liste des inscrit");
    ui->scroll->show();
}
void Inscription::elementCliquee(int li , int co)
{
    vector<int> tmp;
    tmp.push_back(li);
    tmp.push_back(co);

    setElementClic(tmp);
    ui->actionmodifierSLC->setEnabled(true);
    ui->actionsupprSLC->setEnabled(true);
}
vector<int> Inscription::getElementClic()
{
    return (elementClic);
}
void Inscription::setElementClic(vector<int> coord)
{
    elementClic = coord;
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
        supprimer->setShortcut(QKeySequence("Ctrl+S"));
        supprimer->setShortcutVisibleInContextMenu(true);
        modifier = menu.addAction("modifier");
        modifier->setShortcut(QKeySequence("Ctrl+D"));
        modifier->setShortcutVisibleInContextMenu(true);
        choix = menu.exec(tableau->viewport()->mapToGlobal(position));

        if(choix == supprimer)
        {
            sauveHisto();
            liste.erase(liste.begin() + nLigne);
            messageStatBar((liste.begin() + nLigne)->getNom() + " : personne suprimé");
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
            sauveHisto();
            liste.erase(liste.begin() + nLigne);
            lister();
        }
    }
}
void Inscription::setModifier(bool etat)
{
    modifier = etat;
}
void Inscription::tableauEditer(QTableWidgetItem *element)
{
    int ligne , colonne , age;
    bool ok;
    QString valeur;
    string test;

    ligne = element->row();
    colonne = element->column();

    // Vérifier que la ligne correspond à un élément existant dans la liste
    if((ligne >= 0) && (ligne < (int)liste.size()))
    {
        sauveHisto();

        valeur = element->text().trimmed();
        if(valeur.length() == 0)
        {
            QMessageBox::warning(this , "erreur" , "Ne peut pas etre vide !");
        }
        else
        {
            test = valeur.toStdString();
            switch(colonne)
            {
                case 0:
                {
                    if(isdigit(test[0]))
                    {
                        QMessageBox::warning(this , "Erreur" , "Un nom ne peut pas commencer par un nombre");
                        lister();
                    }
                    else
                    {
                        liste[ligne].setNom(valeur);
                        messageStatBar("nom edité ligne n°" + QString::number(ligne+1));
                    }
                    break;
                }
                case 1:
                {
                    if(isdigit(test[0]))
                    {
                        QMessageBox::warning(this , "Erreur" , "Un prenom ne peut pas commencer par un nombre");
                        lister();
                    }
                    else
                    {
                        liste[ligne].setPrenom(valeur);
                        messageStatBar("prenom edité ligne n°" + QString::number(ligne+1));
                    }
                    break;
                }
                case 2:
                {
                    age = valeur.toInt(&ok);
                    if (ok && (age > 0))
                    {
                        liste[ligne].setAge(age);
                        messageStatBar("valeur : age invalide ligne n°" + QString::number(ligne+1));
                    }
                    else
                    {
                        // Valeur invalide : remettre l'ancienne valeur dans le tableau
                        tableau->blockSignals(true);
                        valeur = QString::number(liste[ligne].getAge());
                        element->setText(valeur);
                        messageStatBar("age edité ligne n°" + QString::number(ligne+1));
                        tableau->blockSignals(false);
                    }
                    break;
                }
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
        sauveHisto();
        liste.erase(iPers);
        messageStatBar(iPers->getNom() + " a été supprimé!");
    }
    else
    {
        messageStatBar("Introuvable : Aucune personne correspondant !");
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
void Inscription::sauveHisto()
{
    historique.push_back(liste);
    ui->actionAnnuler->setEnabled(true);
    histoRetablir.clear();
    ui->actionRetablir->setEnabled(false);
}
void Inscription::annuler()
{
    if(historique.size() != 0)
    {
        histoRetablir.push_back(liste);
        ui->actionRetablir->setEnabled(true);
        liste = historique.back();
        historique.pop_back();

        lister();
    }

    if(historique.size() == 0)
    {
        ui->actionAnnuler->setEnabled(false);
    }
}
void Inscription::retablir()
{
    // Restaurer l'état depuis l'historique de rétablissement
    if(histoRetablir.size() != 0)
    {
        historique.push_back(liste);
        ui->actionAnnuler->setEnabled(true);
        liste = histoRetablir.back();
        histoRetablir.pop_back();

        lister();
    }

    if(histoRetablir.size() == 0)
    {
        ui->actionRetablir->setEnabled(false);
    }
}
void Inscription::enregistrerFichier()
{
    QString nomFichier , qchaine;
    string fichier , chaine;
    ofstream sorti;
    vector<Personne>::iterator iPers;

    if(nFichier == "sans_titre.csv")
    {
        nomFichier = QFileDialog::getSaveFileName(this , "Sauvegarder en fichier le feuille" , QString::fromStdString(nFichier) , "filtre csv (*.csv)");
        nFichier = nomFichier.toStdString();
        setNFichier(nFichier);
        modifier = false;
    }
    else
    {
        nomFichier = QString::fromStdString(nFichier);
    }

    if(nomFichier.length() == 0)
    {
        messageStatBar("annulation : Aucun nom de fichier n'a été introduit !");
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
        messageStatBar("Succès : Liste sauvegardée avec succès !");
    }
    else
    {
        messageStatBar("Erreur : Impossible d'ouvrir le fichier pour l'écriture !");
    }

}
void Inscription::ouvrirFichier()
{
    QString nomFichier, nom, prenom , message;
    string fichier, chaine;
    vector<vector <Personne>> tmp , tmpR;
    vector<string> champs;
    ifstream entree;
    int age , nLigne , erreur;
    Personne pers;

    nomFichier = QFileDialog::getOpenFileName(this , "Importez un fichier l'editeur" , "" , "filtre csv (*.csv)");
    if(nomFichier.length() == 0)
    {
        messageStatBar("annulation : Aucun fichier n'a été selectionné !");
        return;
    }
    if(!(nomFichier.isEmpty()))
    {
        fichier = nomFichier.toStdString();

        entree.open(fichier);

        if(entree)
        {
            setNFichier(fichier);
            modifier = false;

            // preserver l'historique en cas d'erreur du lecture de fichier
            historique.push_back(liste);
            tmp = historique;
            tmpR = histoRetablir;
            // pour voir les etats de modification
            historique.clear();
            histoRetablir.clear();
            liste.clear();

            nLigne = 1;
            erreur = 0;
            while(getline(entree, chaine))
            {
                champs = separe(',' , chaine);
                if(std::size(champs) == 3)
                {
                    nom = QString::fromStdString(champs.at(0));
                    prenom = QString::fromStdString(champs.at(1));
                    age = atoi(champs.at(2).c_str());
                    if((age == 0) || (!isalpha(nom.toStdString().at(0))) || (!isalpha(prenom.toStdString().at(0))))
                    {
                        message = "Erreur a la ligne : " + QString::number(nLigne) + "\n>> " + QString::fromStdString(chaine);
                        QMessageBox::warning(this , "Erreur de nommage" , message);

                        erreur = 1;
                        break ; // Arreter la lecture
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

                    erreur = 1;
                    break ; // Arreter la lecture
                }
                nLigne++;
            }
            entree.close();
            if(erreur == 1)
            {
                // retourner à l'ancien liste en cas d'erreur
                historique = tmp;
                histoRetablir = tmpR;
                liste = historique.back();
                historique.pop_back();
                modifier = estModifier();
            }
            lister();
        }
        else
        {
            messageStatBar("Erreur : Impossible d'ouvrir le fichier !");
        }
    }


    if(historique.size() != 0)
    {
        ui->actionAnnuler->setEnabled(true);
    }
    if(histoRetablir.size() != 0)
    {
        ui->actionRetablir->setEnabled(true);
    }
}
void Inscription::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reponse ;


    if(modifier)
    {
        reponse = QMessageBox::question(this , "Quitter" , "Voulez-vous enregistrer avant de quitter ?" , QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
        if(reponse == QMessageBox::Yes)
        {
            enregistrerFichier();
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
void Inscription::on_actionEnregistrer_sous_triggered()
{
    QString nomFichier , qchaine;
    string fichier , chaine;
    ofstream sorti;
    vector<Personne>::iterator iPers;

    nomFichier = QFileDialog::getSaveFileName(this , "Sauvegarder en fichier le feuille" , QString::fromStdString(nFichier) , "filtre csv (*.csv)");

    if(nomFichier.length() == 0)
    {
        messageStatBar("annulation : Aucun nom de fichier n'a été introduit !");
        return;
    }
    setNFichier(nomFichier.toStdString());
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
        messageStatBar("Succès : Liste sauvegardée avec succès !");
    }
    else
    {
        messageStatBar("Erreur : Impossible d'ouvrir le fichier pour l'écriture !");
    }
}
void Inscription::messageStatBar(QString message)
{
    ui->statusBar->showMessage(message , 3000);
}