#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>

#define M 15
#define N 10
#define J 2
#define K 10
using namespace std;

int moves[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
char Chale[M][N];
int flaites[J][2];
int moderados[K][2];
int rats[2][2];
mutex mu;
condition_variable cv;
bool p = false;
bool s = false;
bool sem = true;

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

int randomPosition(){
    srand(rdtsc());
    return (rand() % N);
}

void printChale(){
    int x, y;
    cout << endl;
    for(x = 0; x < M; ++x){
        for(y = 0; y < N; ++y){
            printf("%c%c", Chale[x][y], (y == N-1) ? '\n':' ');
        }
    }
    cout << endl;
}


void init(){ //function to initialize Chale with rats and citizens separated
    int x, y, i, j;
    bool put = 0;

    for(x = 0; x < M; ++x){ //Chale full of '*'
        for(y = 0; y < N; ++y){
            Chale[x][y] = '*';
        }
    }
        
    for(x = 0; x < J; ++x) {
        i = randomPosition();
        j = randomPosition();
        if(Chale[i][j] == '*'){ 
            Chale[i][j] = 'F';  //Flaite
            flaites[x][0] = i;
            flaites[x][1] = j;
        }
        else --x;
    }

    for(x = 0; x < K; ++x) {
        i = randomPosition();
        j = randomPosition();
        if(Chale[i][j] == '*'){
            Chale[i][j] = 'M';  //Moderado
            moderados[x][0] = i;
            moderados[x][1] = j;
        }
        else --x;
    }

    while(true){
        i = randomPosition();
        j = randomPosition();
        if(!put){
            if(Chale[i][j] == '*'){
                rats[put][0] = i;
                rats[put][1] = j;
                Chale[i][j] = 'P'; //Piñata
                put = true;
            }
        }
        else{
            if(Chale[i][j] == '*'){
                rats[put][0] = i;
                rats[put][1] = j;
                Chale[i][j] = 'S'; //Sandwich
                break;
            }
            else{
                if(Chale[i][j] == 'P'){
                    rats[put][0] = i;
                    rats[put][1] = j;
                    Chale[i][j] = 'R'; //R, means Piñata + Sandwich
                    break;
                }
            }
        }
    }
}

class Monitor{
    public:
        Monitor(){
        }
        bool semaphore(){
            return sem;
        }
        void *flaiteTurn(){
            int move,i,j,dice;
            for(int x=0;x<J;x++){
                sem = true;
                unique_lock<mutex> locker(mu);
                cv.wait(locker,std::bind(Monitor::semaphore(),m));
                dice = randomPosition() % 6;
                if(dice == 2){
                    cout << "Flaite " << x+1 << " didn't move this turn." << endl;
                }
                else{
                    while(true){
                        move = randomPosition() % 4;
                        i = flaites[x][0] + moves[move][0];
                        j = flaites[x][1] + moves[move][1];
                        if(i > M-1 || i < 0 || j > N-1 || j < 0){
                            cout << "Invalid move for flaite " << x+1 << ". Lets try again..." << endl;
                            continue;
                        }
                        else{
                            if(Chale[i][j] == 'F' || Chale[i][j] == 'M'){
                                cout << "Citizen right there. Lets try again for flaite " << x+1 <<"..." << endl;
                                continue;
                            }
                            else{
                                cout << "Flaite " << x+1 << " move this turn." << endl;
                                Chale[flaites[x][0]][flaites[x][1]] = 'X';
                                flaites[x][0] = i;
                                flaites[x][1] = j;
                                break;
                            }
                        }
                    }
                    if(Chale[i][j] == 'P'){
                        cout << "Florian Pinata now is in the hospital." << endl;
                        p = true;
                    }
                    else if(Chale[i][j] == 'S'){
                        cout << "Ulises Sandwich now is in the hospital." << endl;
                        s = true;
                    }
                    else if(Chale[i][j] == 'R'){
                        cout << "Florian Pinata and Ulises Sandwich now are in the hospital..." << endl;
                        p = true;
                        s = true;
                    }
                    Chale[i][j] = 'F';
                }   
                printChale();
                cv.notify_one();
                sem = false;
            }
        }

        void *pinataTurn(){
            int move,i,j,dice;
            sem = true;
            unique_lock<mutex> locker(mu);
            cv.wait(locker,std::bind(Monitor::semaphore(),this));
            dice = randomPosition() % 6;
            if(dice == 2 && Chale[rats[0][0]][rats[0][1]]!='N'){
                cout << "Florian Pinata didn't move this turn." << endl;
                return NULL;
            }
            else{
                while(true){
                    move = randomPosition() % 4;
                    i = rats[0][0] + moves[move][0];
                    j = rats[0][1] + moves[move][1];
                    if(i > M-1 || i < 0 || j > N-1 || j < 0){
                        cout << "Invalid move for Florian Pinata. Lets try again..." << endl;
                        continue;
                    }
                    else{
                        if(Chale[i][j] == 'X'){
                            cout << "A flaite passes through here recently. Lets try again for Florian Pinata..." << endl;
                            continue;
                        }
                        else{
                            cout << "Florian Pinata did move this turn." << endl;
                            if(Chale[rats[0][0]][rats[0][1]] = 'P'){
                                Chale[rats[0][0]][rats[0][1]] = '*';
                            }
                            else if(Chale[rats[0][0]][rats[0][1]] = 'N'){
                                Chale[rats[0][0]][rats[0][1]] = 'M';
                            }
                            else if(Chale[rats[0][0]][rats[0][1]] == 'R'){
                                Chale[rats[0][0]][rats[0][1]] = 'S';
                            }
                            rats[0][0] = i;
                            rats[0][1] = j;
                            break;
                        }
                    }
                }
                if(Chale[i][j] == 'M'){
                    Chale[i][j] = 'N';
                    if(dice == 3){
                        cout << "Florian Pinata escaped from a moderado." << endl;
                        sem = false;
                        unique_lock<mutex> locker(mu);
                        pinataTurn();
                        cv.notify_one();
                        sem = true;
                        cv.wait(locker,std::bind(Monitor::semaphore(),this));
                    }
                    else{
                        cout << "Florian Pinata now is writing a new document until end of times..." << endl;
                        Chale[i][j] = 'M';
                        p = true;
                    }
                }
                else if(Chale[i][j] == 'F'){
                    cout << "Florian Pinata now is in the hospital." << endl;
                    p = true;
                }
                else if(Chale[i][j] == 'S'){
                    Chale[i][j] = 'R';
                }
                else{
                    Chale[i][j] = 'P';
                }
            }
            printChale();
            cv.notify_one();
            sem = false;
        }

        void *sandwichTurn(){
            int move,i,j,dice;
            sem = true;
            unique_lock<mutex> locker(mu);
            cv.wait(locker,std::bind(Monitor::semaphore(),this));
            dice = randomPosition() % 6;
            if(dice == 2 && Chale[rats[1][0]][rats[1][1]]!='W'){
                cout << "Ulises Sandwich didn't move this turn." << endl;
                return NULL;
            }
            else{
                while(true){
                    move = randomPosition() % 4;
                    i = rats[1][0] + moves[move][0];
                    j = rats[1][1] + moves[move][1];
                    if(i > M-1 || i < 0 || j > N-1 || j < 0){
                        cout << "Invalid move for Ulises Sandwich. Lets try again..." << endl;
                        continue;
                    }
                    else{
                        if(Chale[i][j] == 'X'){
                            cout << "A flaite passes through here recently. Lets try again for Ulises Sandwich..." << endl;
                            continue;
                        }
                        else{
                            cout << "Ulises Sandwich did move this turn." << endl;
                            if(Chale[rats[1][0]][rats[1][1]] = 'S'){
                                Chale[rats[1][0]][rats[1][1]] = '*';
                            }
                            else if(Chale[rats[1][0]][rats[1][1]] == 'R'){
                                Chale[rats[1][0]][rats[1][1]] = 'P';
                            }
                            else if(Chale[rats[1][0]][rats[1][1]] = 'W'){
                                Chale[rats[1][0]][rats[1][1]] = 'M';
                            }
                            rats[1][0] = i;
                            rats[1][1] = j;
                            break;
                        }
                    }
                }
                if(Chale[i][j] == 'M'){
                    Chale[i][j] = 'W';
                    if(dice == 3){
                        cout << "Ulises Sandwich escaped from a moderado." << endl;
                        sem = false;
                        unique_lock<mutex> locker(mu);
                        sandwichTurn();
                        cv.notify_one();
                        sem = true;
                        cv.wait(locker,std::bind(Monitor::semaphore(),this));
                    }
                    else{
                        Chale[i][j] = 'M';
                        cout << "Ulises Sandwich now is writing a new document until end of times..." << endl;
                        s = true;
                    }
                }
                else if(Chale[i][j] == 'F'){
                    cout << "Ulises Sandwich now is in the hospital." << endl;
                    s = true;
                }
                else if(Chale[i][j] == 'P'){
                    Chale[i][j] = 'R';
                }
                else{
                    Chale[i][j] = 'S';
                }
            }
            printChale();
            cv.notify_one();
            sem = false;
        }

        void *moderadoTurn(){
            int move,i,j,dice;
            for(int x=0;x<K;x++){
                sem = true;
                unique_lock<mutex> locker(mu);
                cv.wait(locker,std::bind(Monitor::semaphore(),this));
                dice = randomPosition() % 6;
                if(dice == 2){
                    cout << "Moderado number " << x+1 << " didn't move this turn." << endl;
                }
                else{
                    while(true){
                        move = randomPosition() % 4;
                        i = moderados[x][0] + moves[move][0];
                        j = moderados[x][1] + moves[move][1];
                        if(i > M-1 || i < 0 || j > N-1 || j < 0){
                            cout << "Invalid move for moderado " << x+1 << ". Lets try again..." << endl;
                            continue;
                        }
                        else{
                            if(Chale[i][j] == 'X' || Chale[i][j] == 'M' || Chale[i][j] == 'F'){
                                cout << "Citizen right there or a flaite passes through here recently. Lets try again for moderado " << x+1 <<"..." << endl;
                                continue;
                            }
                            else{
                                cout << "Moderado number " << x+1 << " did move this turn." << endl;
                                Chale[moderados[x][0]][moderados[x][1]] = '*';
                                moderados[x][0] = i;
                                moderados[x][1] = j;
                                break;
                            }
                        }
                    }
                    if(Chale[i][j] == 'P'){
                        Chale[i][j] = 'N';
                        if(dice == 3){
                            cout << "Florian Pinata escaped from a moderado." << endl;
                            sem = false;
                            unique_lock<mutex> locker(mu);
                            pinataTurn();
                            cv.notify_one();
                            sem = true;
                            cv.wait(locker,std::bind(Monitor::semaphore(),this));
                        }
                        else{
                            Chale[i][j] == 'M';
                            cout << "Florian Pinata now is writing a new document until end of times..." << endl;
                            p = true;
                        }
                    }
                    else if(Chale[i][j] == 'S'){
                        Chale[i][j] = 'W';
                        if(dice == 3){
                            cout << "Ulises Sandwich escaped from a moderado." << endl;
                            sem = false;
                            unique_lock<mutex> locker(mu);
                            sandwichTurn();
                            cv.notify_one();
                            sem = true;
                            cv.wait(locker,std::bind(Monitor::semaphore(),this));
                        }
                        else{
                            Chale[i][j] = 'M';
                            cout << "Ulises Sandwich now is writing a new document until end of times..." << endl;
                            s = true;
                        }
                    }
                    else if(Chale[i][j] == 'R'){
                        int dice2;
                        int n=2;
                        while(n>0){
                            dice2 = randomPosition() %6;
                            dice = randomPosition() % 6;
                            if(n % 2 == 0){
                                if(dice == 1 && dice2 == 1){
                                    Chale[i][j] = 'N';
                                    cout << "Florian Pinata escaped from a moderado." << endl;
                                    sem = false;
                                    unique_lock<mutex> locker(mu);
                                    pinataTurn();
                                    cv.notify_one();
                                    sem = true;
                                    cv.wait(locker,std::bind(Monitor::semaphore(),this));
                                }
                                else{
                                    Chale[i][j] = 'M';
                                    cout << "Florian Pinata now is writing a new document until end of times..." << endl;
                                    p = true;
                                }
                            }
                            else{
                                if(dice == 0 && dice2 == 0){
                                    Chale[i][j] = 'W';
                                    cout << "Ulises Sandwich escaped from a moderado." << endl;
                                    sem = false;
                                    unique_lock<mutex> locker(mu);
                                    sandwichTurn();
                                    cv.notify_one();
                                    sem = true;
                                    cv.wait(locker,std::bind(Monitor::semaphore(),this));
                                }
                                else{
                                    Chale[i][j] = 'M';
                                    cout << "Ulises Sandwich now is writing a new document until end of times..." << endl;
                                    s = true;
                                }
                            }
                        n--;
                        }
                    }
                    else{
                        Chale[i][j] = 'M';
                    }
                }
                cv.notify_one();
                sem = false;
            }
            printChale();
        }

        void *citizensTurns(void *param){
            while(!s && !p){
                moderadoTurn();
                flaiteTurn();
            }
        }

        void *ratsTurns(void *param){
            while(!p || !s){
                if(!p){
                    pinataTurn();
                }
                if(!s){
                    sandwichTurn();
                }
            }
        }
};

int main(){
    Monitor *mon = new Monitor();
    thread ct (&Monitor::citizensTurns,mon);
    thread rs (&Monitor::ratsTurns, mon);
    ct.join();
    rs.join();
    return 0;
}