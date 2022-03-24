#include <condition_variable>
#include <iostream>
#include <thread>
#include "sync_var.h"


Command *command = nullptr; //init
Data *buffor[2] = {nullptr, nullptr}; //init

void Monitor::set_command(Command *c){ //ustawiamy komanda w monitorze
        std::unique_lock<std::mutex> lock(guard_mutex); //jesli locked to unlocked, jesli unlocked to lock biblioteka standardowa
        // wspolpracuje z muexem
        get_com.wait(lock, []{return command == nullptr;}); // 1 na 0 jak semafory
        command = c; // etujemy i dlatego
        set_com.notify_all(); // identycznie jak z semaforami set na 1 
    }
Command* Monitor::get_command(){ // pobieramy commanda  tuaj 
       std::unique_lock<std::mutex> lock(guard_mutex);
       set_com.wait(lock, []{return command!= nullptr;}); // set na 0 
       Command *c = command;
       command = nullptr; // oprozniamy
       get_com.notify_all(); // get na 1 s
       return c;

    }

void Monitor::set_data(Data* value, int idx){
        std::unique_lock<std::mutex> lock(guard_mutex);
        get_val.wait(lock, [idx]{return buffor[idx] == nullptr;});
        buffor[idx] = value;
        set_val.notify_all();
    }

Data* Monitor::get_data(int idx){
       std::unique_lock<std::mutex> lock(guard_mutex);
       set_val.wait(lock, [idx]{return buffor[idx]!= nullptr;});
       Data *d = buffor[idx];
       buffor[idx] = nullptr;
       get_val.notify_all();
       return d;

    }

