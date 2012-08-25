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

using namespace std;

const string COMMANDER("/SpaceConcordia/commander.exe"); //process name for the commander
const string DISPATCHER("/SpaceConcordia/dispatcher.exe");
const int SLEEP_TIME = 300;

int g_dispatcher_alive = 1;
int g_commander_alive  = 1;

void kill_all_pids(string pids) {       
    if (!pids.empty()) {
        pid_t pid = 0;
        char* pid_str = strtok(pids.c_str(), " ");
        
        while(pid_str != NULL) {
            pid = atoi(pid_str);
            kill(pid, SIGKILL);
            pid_str = strtok(NULL, " "); //grab the next string by starting where we left off
        }    
    }
}

void reset_process(const string cmd) {    
    string command("pidof ");
    command.append(cmd);
    
    //clean up running instances of cmd
    FILE* fp = popen(command, "r");
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
        
    //start the process
    fp = popen(cmd, "r"); 
    if (fp != NULL) {       
        pclose(fp);
    }
    else {
        //log errorzor
    }
}

void sig_handler_USR1() {
        g_commander_alive = 0;
}

void sig_handler_USR2() {
        g_dispatcher_alive = 0;
}

int main() {
    signal(SIGUSR1, sig_handler_USR1);
    signal(SIGUSR2, sig_handler_USR2);

    sleep(SLEEP_TIME);

    if (g_dispatcher_alive == 1) {
            reset_process(COMMANDER);
    } 
    if (g_commander_alive == 1) {
            reset_process(DISPATCHER);
    }  

    return 0; 
}