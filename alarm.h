#ifndef PILL_DISPENSER_ALARM_H
#define PILL_DISPENSER_ALARM_H

#include <stdio.h>

class Alarm{
public:

    Alarm(){
      time = 0;
      for (uint8_t i = 0; i < 7; i++){
        day_of_alarm[i] = false;
      }
      chute_no = 0;
    }
    
    Alarm(uint32_t tmptime, bool tempdayofweek[], uint8_t chuteno) {
    this->time = tmptime;
    for (int i = 0; i < 7; i++){
      this->day_of_alarm[i] = tempdayofweek[i];
    }
    this->chute_no = chuteno;
    }

    //setters
    void setAlarmDay(uint8_t day, bool status){
      day_of_alarm[day]=status;
    }

    //modifiers
    void changeAlarmDay(uint8_t day); //input numbers 0-6 for monday-sunday
    void changeAlarmTime(uint32_t tmptime); //input time in seconds
    void changeAlarmChute(uint8_t chute); //input number 1-6 for chutes
    
    //getters
    bool getAlarmDays(uint8_t day);
    uint32_t getAlarmTime();
    uint8_t getChuteNo();

private:
    uint32_t time; //expressed in minutes
    bool day_of_alarm[7];//index 0 = monday, index 6 = sunday. val of true means set, val of false means not set
    uint8_t chute_no; //numbers 1-6
};
#endif //PILL_DISPENSER_ALARM_H
