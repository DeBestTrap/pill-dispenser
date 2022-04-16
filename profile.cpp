#include <stdio.h>
#include <string.h>

#include "profile.h"
#include "alarm.h"
#include <Adafruit_Fingerprint.h>


char Profile::setName(char name[]){
    strcpy(this->name, name);
}

int Profile::setFingerprint(uint8_t fingerprint){
    this->fingerprint = fingerprint;

}

void Profile::setNewAlarm(Alarm tempAlarm) {
    for (uint8_t i = 0; i < 10; i++){
      if (alarms[i].getChuteNo() == 0){
        alarms[i] = tempAlarm;
        break;
      }
    }
}


