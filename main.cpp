#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sync_var.h"
#include<thread>
#include <csignal>
// typedef struct { // struktura z zadania 
//     int code;  // kod 0 lub 1 
//     int index; // indeks bufora
//     int optional; // bonifacy
// } Command;

// typedef union {
//     unsigned long int i;
//     double d;
// } Data;

static volatile int producerRunning = 1; // volatile  zmienna moze byc zmieniana z poza programu
static volatile int consumersRunning = 1; // 1 
static pthread_t producerThread;
static pthread_t bonifacyThread;
static pthread_t randomThread;

Monitor m;



static unsigned long fibonacci(unsigned long n) {
    const double a = sqrt(5.0);
    const double b = 1.0 / a;
    return b*pow((1.0 + a)/2, n) - b*pow((1.0 - a)/2, n);
} // 


static void* producerMain(void *args) {
    while(producerRunning) {
        Command* c = m.get_command(); // pobieramy instrukcje jesli tam 1 
      
        if(!c) {
            printf("[prod] This should never happen\n");
            continue;
        } // obsluga bledu 

        if(c->code == -1) { // wyjdzie zpetli producenta jesli  123 linijka
            delete c;
            break; // signal Handler zabija
        }

        printf("[prod] Recv cmd %i, %i, %i\n", c->code, c->index, c->optional);

         Data *data = new Data; // Unia na dane

        if(c->code == 0) {
            data->i = fibonacci(c->optional); // wrzucanie do unii danych
        }
        else if(c->code == 1) {
            data->d = (double)rand() / (double)RAND_MAX;
        }

        m.set_data(data,c->index); // buffer tablica sync var, wpisanie danych do bufora do podanego indeksu
        delete c; // zwolnienie rozkazu 
    }
    
    printf("[prod] Exit\n"); // ajk wyjdzie z whila to sie dzieje
    return NULL;
}

static void* bonifacyMain(void *args) {
    while(consumersRunning) {
        Command* c = new Command;
        c->code = 0;
        c->index = 0;
        c->optional = rand() % 100; // indeks liczby bonifacego 
        m.set_command(c); // signal z 0 na 1  patrzy na proces i przypisuje value 

        Data* data = m.get_data(0); // unia czekamy az producent wyprodukuje dane i tutaj je pobieramy

        while(data == NULL) {
            printf("[boni] This should never happen\n");
            data = m.get_data(0); // do aktywnego oczekiwania i mutexow
        };
        
        printf("[boni] Got value: %lu\n", data->i);
        delete data;

        sleep(1);
    }
    
    printf("[boni] Exit\n");
    return NULL;
}

static void* randomMain(void *args) {
    while(consumersRunning) {
        Command* c = new Command;
        c->code = 1;
        c->index = 1;
        m.set_command(c);

        Data* data =  m.get_data(1);

        while(data == NULL) {
            printf("[rand] This should never happen\n");
            data =  m.get_data(1);
        };
        
        printf("[rand] Got value: %f\n", data->d);
        delete data;

        sleep(2);
    }
    
    printf("[rand] Exit\n");
    return NULL;
}

static void signalHandler(int sig) { // od numeru sygnalu
    printf("Exiting...\n"); 
    consumersRunning = 0;// wylaczenie dzialania consumerow
    pthread_join(randomThread, NULL);
    pthread_join(bonifacyThread, NULL); // zakonczenie watku
    producerRunning = 0; // najpierw wylaczamy konsumentow potem producent - logiczne
    // if(syncVarIsAbsent(command)) { 
    //     Command* c = malloc(sizeof(Command));
    Command *c = new Command;
    c->code = -1; // producent pobiera rozkaz i go wykonuje
    //     syncVarSet(command, c); // jesli konsumenci sa wylaczeni to nie ma kto wydawac rozkazow 
    //     // nie ma on prawa zawiesic sie, dlatego jak nie ma rozkazu to wkladamy fakowy rozkaz
    //     // zeby poszedl dalej i sie zakonczyl
    // }
    // producer.join(); // czekamy az producer sie skonczy
    // syncVarDestroy(command); //zwalnianie zmiennych
    // for(int i = 0; i < 2; i++) {
    //     syncVarDestroy(buffer[i]); //zwalnianie bufora
    // }
    pthread_join(producerThread, NULL);
    printf("Exit\n"); //Exit 
    exit(0);
}

int main(void) {
    std::signal(SIGINT, signalHandler); // ustawienie handlera - funckja wywolana z ctrl c w terminalu SIGINT
    srand(time(NULL)); // init randomowosci

    // if(!(command = syncVarCreate())) { // tworzenie zsynchronizowanyhc zmiennych
    //     return EXIT_FAILURE;
    // }
   


    // for(int i = 0; i < 2; i++) {
    //     if(!(buffer[i] = syncVarCreate())) { // jesli jest pusty kill
    //         return EXIT_FAILURE;
    //     }
    // }
    // std::thread producer(producerMain);
    // std::thread bonifacy(bonifacyMain);
    // std::thread random(randomMain);

    
    // bonifacy.join();
    // random.join();
    // producer.join();
    pthread_create(&producerThread, NULL, producerMain, NULL); // uchwyt watku i wykonywanie watkutworzenie watku i wywolywanie funkcji
    pthread_create(&bonifacyThread, NULL, bonifacyMain, NULL);
    pthread_create(&randomThread, NULL, randomMain, NULL); 
    pthread_join(producerThread, NULL);  // czeka az watki zakoncza prace
    pthread_join(bonifacyThread, NULL);
    pthread_join(randomThread, NULL);
    return EXIT_SUCCESS;
}
