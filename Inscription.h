#ifndef INSCRIPTION_H
#define INSCRIPTION_H

#include "Personne.h"
#include <QMainWindow>
#include <QTableWidget>
#include <vector>
#include <QPoint>

using namespace std ;

QT_BEGIN_NAMESPACE
namespace Ui {
class Inscription;
}
QT_END_NAMESPACE
class Inscription : public QMainWindow
{
    Q_OBJECT

public:
    explicit Inscription(QWidget *parent = nullptr);
    ~Inscription() override;
    vector<Personne> getListe();
    void setListe(vector<Personne> l);
    int initialise(Personne& individu , bool accepteErreur);
    vector<Personne> trouve(Personne individu , vector<Personne> listeP);
    vector<string> separe(char sep , string chaine);
    bool estModifier();
    string getNFichier();
    void setNFichier(string nomFic);
    void sauveHisto();

private slots:
    void lister();
    void ajouter();
    void supprimer();
    void rechercher();
    void tableauEditer(QTableWidgetItem *element);
    void Annuler();
    void Retablir();
    void OuvrirFichier();
    void EnregistrerFichier();
    void closeEvent(QCloseEvent *event) override;
    void menuContextuel(const QPoint &position);
    void deplaceCurseur();

private:
    Ui::Inscription *ui;
    vector<Personne> liste;
    QTableWidget *tableau;
    vector<vector<Personne>> historique;
    vector<vector<Personne>> histoRetablir;
    string nFichier;

};
#endif
