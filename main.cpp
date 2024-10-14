#include <iostream>
#include <mutex>
#include <thread>
#include <semaphore>
#include <vector>

using namespace std;

// VARIABLES GLOBALES

int patients = 0; // Nombre de clients dans la clinique
bool medicIsSleeping; // True si le docteur dort
vector<thread> patientsThreads;

// PARAMETRES

int n = 3; // Nombre de chaises
int tmp_takeCareOfPatient = 1; // Temps que prend le médecin pour s'occuper d'un patient
int tmp_newPatient = 2; // Intervalle de temps entre chaque nouveau patient

// RESSOURCES CRITIQUES
mutex mut;

void takeCareOfPatient()
{
    cout << "Le médecin s'occupe d'un patient\n";
    sleep(tmp_takeCareOfPatient);
}

void medicNap()
{
    medicIsSleeping = true;

    cout << "Le médecin fait la sieste\n";
    while(medicIsSleeping)
    {
        sleep(1);
    }
    cout << "Le médecin se réveille\n";

    return;
}

void medicRoutine()
{
    while(true)
    {
        mut.lock();
        if(patients == 0) // Si il n'y a pas de patient
        {
            medicNap();
        }
        else
        {
            takeCareOfPatient();
            patients -= 1;
        }
        mut.unlock();
    }
}

void wakeUpMedic()
{
    cout << "Le patient " << this_thread::get_id() << " réveille le médecin\n";
    medicIsSleeping = false;
}

void patientRoutine()
{
    mut.lock();
    // Le patient arrive dans la clinique
    if(patients == n)
    {
        cout << "La clinique ne peut pas accueillir le patient " << this_thread::get_id() << endl;
        return;
    }

    if(patients == 0 && medicIsSleeping)
    {
        wakeUpMedic();
    }

    // Le patient attend son tour
    cout << "Le patient " << this_thread::get_id() << " va dans la file d'attente\n";
    patients += 1;
    cout << patients << " patients dans la file d'attente\n";
    mut.unlock();
}

int main()
{
    thread medic (medicRoutine);

    while(true)
    {
        // Faire rentrer des patients régulièrement
        patientsThreads.push_back(thread(patientRoutine));
        sleep(tmp_newPatient);
    }

    return 0;
}
