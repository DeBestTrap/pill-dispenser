#ifndef profile_h
#define profile_h

#include <stdio.h>
#include "alarm.h"

//make a class that can be used to store a profile
class Profile{
    public:
        
        //constructor
        Profile() {
          for (uint8_t i = 0; i < 10; i++){
            alarms[i] = Alarm();
          }
          this->fingerprint = 0;
        }

        Profile(char name[], int fingerprint){
          strcpy(this->name, name);
          this->fingerprint = fingerprint;
          for (uint8_t i = 0; i < 10; i++){
            alarms[i] = Alarm();
          }
        }

        //getters
        char* getName() {return name;}
        int getFingerprint() {return fingerprint;}
        Alarm getAlarm() {return alarms[0];}

        //setters
        char setName(char name[]);
        int setFingerprint(int fingerprint);
        void setNewAlarm (Alarm tempAlarm);
        

    private:
        char name[10];
        uint8_t fingerprint; 
        Alarm alarms[10];

};

#endif
