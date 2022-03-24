#ifndef SYNC_VAR_H
#define SYNC_VAR_H
#include <stdbool.h>
// #include <semaphore.h>
#include <condition_variable>
#include <iostream>
#include <thread>

// typedef struct { // struktura z zadania 
//     int code;  // kod 0 lub 1 
//     int index; // indeks bufora
//     int optional; // bonifacy
// } Command;

union Data{
    unsigned long int i;
    double d;
};

struct Command { // struktura command z zadania 
    int code;  //   boni czy rand czy wylacz
    int index; // inde bufora
    int optional; // do bonifacego
};

class Monitor{
    private:
        std::mutex guard_mutex; // semafor binarny 
        std::condition_variable set_val, get_val, set_com, get_com; //4 funkcje ustawiajace
    public:
        void set_command(Command *c); // ustaw w strukturze
        Command *get_command(); // pobierz ze struktury
        void set_data(Data* value, int idx); // ustaw w dacie - na indexie (bufor)
        Data *get_data(int idx); // pobierz

};

#endif