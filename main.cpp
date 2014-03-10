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
#include "shakespeare.h"

using namespace std;

#ifdef PC
    #undef __FOLDER__
    #define __FOLDER__ "/home/jamg85/space"
    const string BABY_CRON(__FOLDER__"/apps/current/baby-cron/baby-cron");
    const string SPACE_COMMANDER(__FOLDER__"/apps/current/space-commander/space-commander");
    const string PID_FILE(__FOLDER__"/watch-puppy.pid");
    const string LOGS_FOLDER(__FOLDER__"/logs/");
#else
    const string BABY_CRON("/home/apps/current/baby-cron/baby-cron");
    const string SPACE_COMMANDER("/home/apps/current/space-commander/space-commander");
    const string PID_FILE("/home/pids/watch-puppy.pid");
    const string LOGS_FOLDER("/home/logs/");
#endif

const int SLEEP_TIME = 60;
const int ALIVE      = 0;
const int DEAD       = 1;

// Track who did not talk to us 
int g_baby_cron_alive        = DEAD;
int g_space_commander_alive  = DEAD;

FILE* g_fp_log = NULL;

void write_current_pid() {
    const int BUFFER_SIZE    = 10;
    char buffer[BUFFER_SIZE] = {0};

    //Programs that need to signal Watch-Puppy will grab its pid from this file
    string filename = PID_FILE;
    pid_t pid = getpid();
    FILE* fp  = fopen(filename.c_str(), "w");

    sprintf(buffer, "%d", pid);

    if (fp != NULL) {
        fwrite(buffer, BUFFER_SIZE, sizeof(char), fp);
        fclose(fp);
    }
}

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
        Shakespeare::log(g_fp_log, Shakespeare::ERROR, "Watch-Puppy", "Couldn't kill pids");
    }        
   
    pid_t pid = fork();
    if (pid < 0) {
       cout << "oh oh";
    }
    else if (pid == 0) {        
        int errno = execl(process.c_str(), process.c_str(), (char*)NULL);       
        if (errno == -1) {
            Shakespeare::log(g_fp_log, Shakespeare::ERROR, "Watch-Puppy", "Couldn't launch: " + process);
            exit(0);
        }
    }          
}

void sig_handler_USR1(int signum) {
        cout << "Watch Puppy: Woof woof delicious baby-cron!" << endl;
        cout.flush();
        g_baby_cron_alive = ALIVE;
}

void sig_handler_USR2(int signum) {
        cout << "Earth is talking to me! Don't panic!" << endl;
        cout.flush();
        g_space_commander_alive = ALIVE;
}


void signal_hardware_watch_daddy() {
    FILE* inaSysFile;
    static char pin_state = 0;

    inaSysFile=fopen("/dev/gpios/consat/3V3_CTL/value","w");
    if (inaSysFile!=NULL) {
        fwrite(&pin_state, sizeof(char) * 1, 1, inaSysFile);
        if (pin_state == 0) {
            pin_state = 1; 
        } 
        else { 
            pin_state = 0; 
        }

        cout << "Pin state: " << (pin_state + 30) << endl;
        cout.flush();
        fclose(inaSysFile);
    } else {
        cout << "Failed" << endl;
        cout.flush();
        // TODO: add shakespeare
    }
}
          
void init_log() {
    string filename = Shakespeare::get_filename(LOGS_FOLDER, "Watch-Puppy", ".log");
    g_fp_log = fopen(filename.c_str(), "a");
}


//-----------------------------------------------------------------------------
// Note:
// To test, do not run it from Netbean. Netbeans becomes the parent so when the
// puppy exits, Netbeans also kill all the children
//-----------------------------------------------------------------------------

int main() {
    write_current_pid();

    init_log();
    log(g_fp_log, Shakespeare::NOTICE, "Watch-Puppy", "Starting");
    fflush(g_fp_log);

    int sleep_remaining = SLEEP_TIME;
        
    signal(SIGUSR1, sig_handler_USR1);
    signal(SIGUSR2, sig_handler_USR2);            
    
    while (true) {
        g_baby_cron_alive       = DEAD;
        g_space_commander_alive = DEAD;

        // Use a loop instead of sleep to wait for puppy before checking for signal.
        // This is to prevent being awake from sleep and by OS checking too early
        while (sleep_remaining > 0) {
            sleep_remaining -= 1;             
            sleep(1);

            if (sleep_remaining % 10 == 0) {
                signal_hardware_watch_daddy();
            }
        }  
        
        if (g_baby_cron_alive == DEAD) {
            reset_process(BABY_CRON);
        }

        if (g_space_commander_alive == DEAD) {
            reset_process(SPACE_COMMANDER);
        }

        sleep_remaining = SLEEP_TIME;
    }
                   
    if (g_fp_log) {
        fclose(g_fp_log);
        g_fp_log = NULL;
    }

    return 0;   
}
