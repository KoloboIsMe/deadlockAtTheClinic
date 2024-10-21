#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
using namespace std;

int chrono_newPatient = 2;

int chairs = 1;
int customers = 0;
mutex mut;
mutex customer;
mutex barber;
mutex customerDone;
mutex barberDone;

void getHairCut(int id)
{
    mut.lock();                 // Vérouille le compteur de clients
    if(customers == chairs)     // Si la salle est pleine
    {                           //
        mut.unlock();           // Libère le compteur de clients et s'en va.
    }                           //
    else                        // Sinon
    {                           //
        customers++;            // Rentre dans le magasin
        customer.lock();        // Vérouille la ressource critique client ?
        barber.lock();          // Vérouille la ressource critique barbier ?
        // Get the haircut      // Se fait couper les cheveux
        customerDone.lock();    // Vérouille la ressource critique customerDone ?
        barberDone.lock();      // Vérouille la ressource critique barberDone ?
        mut.lock();             // Vérouille le compteur de clients
        customers--;            // Sort du magasin
        mut.unlock();           // Libère le compteur de clients
    }

}

void cutHair()
{
    customer.lock();            // Vérouille la ressource critique client ?
    barber.lock();              // Vérouille la ressource critique barbier ?
    // Coupe les cheveux        // Coupe les cheveux du client
    customerDone.lock();        // Vérouille la ressource critique customerDone
    barberDone.lock();          // Vérouille la ressource critique barberDone ?
}

void routine()
{
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
        patientsThreads.push_back(thread(getHairCut, clientId++));
        sleep(chrono_newPatient);
    }

    return 0;
}
