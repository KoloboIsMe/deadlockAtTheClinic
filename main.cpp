#include <iostream>
#include <mutex>
#include <thread>
#include <semaphore>
#include <vector>

using namespace std;

int patients = 0; // Nombre de clients dans la clinique
bool medicIsSleeping; // True si le docteur dort

int n = 5; // Nombre de chaises
int tmp_takeCareOfPatient = 5; // Temps que prend le médecin pour s'occuper d'un patient
int tmp_newPatient = 2; // Intervalle de temps entre chaque nouveau patient

void medicRoutine()
{
    while(true)
    {
        if(patients == 0) // Si il n'y a pas de patient
        {
            medicIsSleeping = true;
            cout << "Le médecin fait la sieste\n";
            while(medicIsSleeping)
            {
                sleep(1);
            }
            cout << "Le médecin se réveille\n";
        }
        else
        {
            cout << "Le médecin s'occupe d'un patient\n";
            sleep(tmp_takeCareOfPatient);
            patients -= 1; // Et le patient sort
        }
    }

}

void patientRoutine()
{
    cout << "Le patient " << this_thread::get_id() << " rentre dans la clinique\n";
    // Le patient arrive dans la clinique
    if(patients == n)
    {
        cout << "La clinique ne peut pas accueillir le patient " << this_thread::get_id() << endl;
        cout << "Le patient " << this_thread::get_id() << " sort de la clinique\n";
        return;
    }

    if(patients == 0 && medicIsSleeping)
    {
        cout << "Le patient " << this_thread::get_id() << " réveille le médecin\n";
        medicIsSleeping = false; // Si la file est vide et le docteur dort, le patient réveille le doteur
    }

    // Le patient attend son tour
    cout << "Le patient " << this_thread::get_id() << " va dans la file d'attente\n";
    patients += 1;
    cout << patients << " patients dans la file d'attente\n";
}

vector<thread> patientsThreads;

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
