#include "alarm.h"
#include <stdio.h>

void Alarm::changeAlarmDay(uint8_t day){
    if (day_of_alarm[day]==0)day_of_alarm[day] = 1;
    else day_of_alarm[day] = 0;
}
void Alarm::changeAlarmTime(uint32_t tmptime){
    time = tmptime;
}
void Alarm::changeAlarmChute(uint8_t chute){
    chute_no = chute;
}

uint8_t Alarm::getAlarmDays(uint8_t day){
    return day_of_alarm[day];
}

uint32_t Alarm::getAlarmTime(){
    return time;
}

uint8_t Alarm::getChuteNo(){
    return chute_no;
}
