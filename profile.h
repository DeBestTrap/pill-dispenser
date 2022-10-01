#ifndef profile_h
#define profile_h

#include <stdio.h>
#include "alarm.h"

//make a class that can be used to store a profile
class Profile{
    public:
        
        //constructor
        Profile() {
          for (uint8_t i = 0; i < 3; i++){
            alarms[i] = Alarm();
          }
          this->id = 0;
        }

        Profile(char name[], int fingerprint){
          strcpy(this->name, name);
          this->id = fingerprint;
          for (uint8_t i = 0; i < 3; i++){
            alarms[i] = Alarm();
          }
        }

        //getters
        char* getName() {return name;}
        uint8_t getFingerprint() {return id;}
        Alarm getAlarm(uint8_t i) {return alarms[i];}
        

        //setters
        char setName(char name[]);
        int setFingerprint();
        void setNewAlarm (Alarm tempAlarm);
        int setFingerprint(uint8_t fingerprint);
        

    private:
        char name[20];
        Alarm alarms[3];
        uint8_t id;
  


};

#endif
