#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
using namespace std;

int chrono_newPatient = 1;

int chairs = 5;
int patients = 0;
mutex mut;
mutex patient;
mutex medic;
mutex patientDone;
mutex medicDone;
mutex medicWakeUp;
bool medicAsleep;


void balk(int id){
    cout << "Patient " << id << ": s'en va car la clinique est pleine" << endl;
    while(true){}
}

void visitClinic(int id)
{
    // Arrivée dans la clinique
    cout << "Patient " << id << ": entre dans la clinique" << endl;

    mut.lock();                 // Vérouille le compteur de patients
    if(patients >= chairs)      // Si la salle est pleine
    {                           //
        mut.unlock();           //      Libère le compteur de patients
        balk(id);               //      Sort de la clinique
    }                           //
    patients++;                 // Rentre dans la clinique
    mut.unlock();               // Dévérouille le compteur de patients

    if (patients == 1 && medicWakeUp.try_lock() == false) {
        cout << "Patient " << id << ": réveille le médecin" << endl;
        medicAsleep = false;
    }

    // Attente et consultation
    cout << "Patient " << id << ": attend pour sa consultation" << endl;

    patient.unlock();           // Déclare qu'il est disponible
    medic.lock();               // Attend que le médecin soit disponible
                                // Consultation ...
    medicDone.lock();           // Attend que le médecin déclare aussi qu'il ait terminé
    patientDone.unlock();       // Déclare qu'il a fini sa consultation

    // Sortie de la clinique
    cout << "Patient " << id << ": sort de la clinique" << endl;

    mut.lock();                 // Vérrouille le compteur de clients
    patients--;                 // Sort de la clinique
    mut.unlock();               // Libère le compteur de clients
}

void takeCareOfPatient()
{
    cout << "Médecin : Attend un patient" << endl;

    if(patient.try_lock() == false) { // Si il n'y a personne dans la file d'attente
        cout << "Médecin : S'endort car il n'y a pas de patient" << endl;
        medicAsleep = true;        // Indique que le barbier est endormi
        while (medicAsleep) {      // Attendre jusqu'à ce qu'il soit réveillé
        }
        cout << "Médecin : Se fait réveiller" << endl;
    }
    medic.unlock();             // Déclare qu'il est disponible
    patient.lock();             // Attend un patient
                                // Consultation en cours ...
    patientDone.lock();         // Attend que le patient déclare qu'il a fini
    medicDone.unlock();         // Termine son travail
}

void routine()
{
    medicWakeUp.unlock();
    while (true)
    {
        takeCareOfPatient();
    }
}

int main()
{
    thread medic(routine);
    vector<thread> patientsThreads;
    int clientId = 1;

    while (true) {
        cout << "Nombre de patients dans la clinique :"<< patients << endl;
        patientsThreads.push_back(thread(visitClinic, clientId++));
        sleep(chrono_newPatient);
    }

    return 0;
}
