#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
using namespace std;

int chrono_newPatient = 1;

int chairs = 5;
int customers = 0;
bool barberAsleep = false;
mutex mut;
mutex customer;
mutex barber;
mutex customerDone;
mutex barberDone;


void balk(int id){
    cout << "Patient " << id << ": s'en va car la clinique est pleine" << endl;
    while(true){}
}

void getHairCut(int id)
{
    // Arrivée dans la clinique
    cout << "Patient " << id << ": entre dans la clinique" << endl;

    mut.lock();                 // Vérouille le compteur de clients
    if(customers >= chairs)     // Si la salle est pleine
    {                           //
        mut.unlock();           //      Libère le compteur de clients
        balk(id);               //      Sort de la clinique
    }                           //
    customers++;                // Rentre dans le magasin
    mut.unlock();               // Dévérouille le compteur de clients

    if (customers == 1 && barberAsleep) {
        cout << "Patient " << id << ": réveille le médecin" << endl;
        barberAsleep = false;
    }

    // Attente et consultation
    cout << "Patient " << id << ": attend pour sa consultation" << endl;

    customer.unlock();          // Déclare qu'il est disponible
    barber.lock();              // Attend que le médecin soit disponible
    // Get the haircut          // Se fait couper les cheveux
    barberDone.lock();          // Attend que le médecin déclare aussi qu'il a terminé
    customerDone.unlock();      // Déclare qu'il a fini sa consultation

    // Sortie de la clinique
    cout << "Patient " << id << ": sort de la clinique" << endl;

    mut.lock();                 // Vérouille le compteur de clients
    customers--;                // Sort de la clinique
    mut.unlock();               // Libère le compteur de clients
}

void cutHair()
{
    cout << "Médecin : Attend un patient" << endl;

    if (customer.try_lock() == false) {
        cout << "Médecin : S'endort car il n'y a pas de patient" << endl;
        barberAsleep = true;        // Indique que le barbier est endormi
        while (barberAsleep) {      // Attendre jusqu'à ce qu'il soit réveillé
            //this_thread::yield();   // Laisse d'autres threads fonctionner
            sleep(1);
        }
        cout << "Médecin : Se fait réveiller" << endl;
    }

    barber.unlock();            // Déclare qu'il est disponible
    customer.lock();            // Attend un client
    // Coupe les cheveux        // Coupe les cheveux du client
    customerDone.lock();        // Attend que le client déclare qu'il a fini
    barberDone.unlock();        // Termine son travail
}

void routine()
{
    barberAsleep = true;
    while (true)
    {
        cutHair();
    }
}

int main()
{
    thread barber(routine);
    vector<thread> patientsThreads;
    int clientId = 1;

    while (true) {
        cout << "Nombre de patients dans la clinique :"<< customers << endl;
        patientsThreads.push_back(thread(getHairCut, clientId++));
        sleep(chrono_newPatient);
    }

    return 0;
}
