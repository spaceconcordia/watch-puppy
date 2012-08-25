/* 
 * File:   main.cpp
 * Author: spaceconcordia
 * Aye! This be the WatchPuppy
 * Created on August 24, 2012, 7:13 PM
 */
#include <string>

using namespace std;

const string COMMANDER("/SpaceConcordia/commander.exe"); //process name for the commander
const string DISPATCHER("/SpaceConcordia/dispatcher.exe");
const int SLEEP_TIME = 300;




#include <signal.h>
#include <cstdlib>
#include <unistd.h>

using namespace std;


void reset_process(const string cmd) {    
    //get pid for full path
        
    //if one or many, kill it or them
    
    //start it
        
}
int dispatcher_alive = 1;
int commander_alive = 1;

void sig_handler_USR1(){
        commander_alive = 0;
}

void sig_handler_USR2(){
        dispatcher_alive = 0;
}

            
    

 int main(){
    
        signal(SIGUSR1, sig_handler_USR1);
        signal(SIGUSR2, sig_handler_USR2);
           
        sleep(SLEEP_TIME);
        
        if (dispatcher_alive == 1){
                reset_process(COMMANDER);
        }
        if (commander_alive == 1){
                reset_process(DISPATCHER);
        }  

        return 0; 
 }