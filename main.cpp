/* 
 * File:   main.cpp
 * Author: spaceconcordia
 * Aye! This be the WatchPuppy
 * Created on August 24, 2012, 7:13 PM
 */
#include <string>
#include <signal.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>


using namespace std;

const string DISPATCHER("/home/CODE/dispatcher.exe");
const int SLEEP_TIME = 10;

int g_dispatcher_alive = 1;
int g_talking_to_earth  = 1;

char* string_to_char_ptr(string str, int size) {
    char* result = (char* )malloc(sizeof(char) * size);
    strcpy(result, str.c_str());
    return result;
}

void kill_all_pids(string pids) {       
    if (!pids.empty()) {
        pid_t pid = 0;
        char* pids_str = string_to_char_ptr(pids, pids.size());        
        char* pid_token = strtok(pids_str, " ");        
        
        while(pid_token != NULL) {
            pid = atoi(pid_token);
            kill(pid, SIGKILL);
            pid_token = strtok(NULL, " "); //grab the next string by starting where we left off
        }    
        
        free(pids_str);
    }
}

void reset_process(const string process) {    
    string command("pidof ");
    command.append(process);
    
    //clean up running instances of process
    FILE* fp = popen(command.c_str(), "r");
    if (fp != NULL) {
        char pids[255] = "";
        fgets(pids, sizeof(pids) - 1, fp);

        fflush(fp);
        pclose(fp);

        kill_all_pids(pids);                                
    }
    else {
        //log errorzor
    }        
   
    pid_t pid = fork();
    if (pid < 0) {
        cout << "oh oh";
    }
    else if (pid == 0) {        
        execl(process.c_str(), NULL);       
    }          
}

void sig_handler_USR1(int signum) {
        cout << "reset dispatcher" << endl;
        g_dispatcher_alive = 0;
}

void sig_handler_USR2(int signum) {
        cout << "talking to earth" << endl;
        g_talking_to_earth = 0;
}

//-----------------------------------------------------------------------------
// Note:
// To test, do not run it from Netbean. Netbeans becomes the parent so when the
// puppy exits, Netbeans also kill all the children
//-----------------------------------------------------------------------------

int main() {
    int sleep_remaining = SLEEP_TIME;
        
    signal(SIGUSR1, sig_handler_USR1);
    signal(SIGUSR2, sig_handler_USR2);            
    
    while (sleep_remaining > 0) {
        sleep_remaining -= 1;             
        cout << sleep_remaining << endl;
        cout.flush();
        sleep(1);
    }       
          
    if (g_talking_to_earth == 1) {
        if (g_dispatcher_alive == 1) {
            reset_process(DISPATCHER);
        }      
    }
    
    return 0;   
}