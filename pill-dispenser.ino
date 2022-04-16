// 4/9/22 Tyler Chan

// Include libraries
#include "Nextion.h"
#include "profile.h"
#include "alarm.h"

#define GREENCOLOR 50712
char weekday_abbrv[7] = {'U', 'M', 'T', 'W', 'R', 'F', 'S'};
uint32_t currentTime = 0;
uint8_t pill_array[6];
String chute_name_array[6];
uint8_t fingerid = 0;

uint32_t prev_millis = 0;
uint8_t current_hour = 0;
uint8_t current_minute = 0;
uint8_t current_day = 0; //0-6, 0: sunday, 6: saturday

// Define Pins
// #define MOTORPIN 3

// Declare your Nextion objects - Example (page id = 0, component id = 1, component name = "b0") 
// ** IMPORTANT **
//   The variable names are going to include the page number to decrease confusion. The names of
//   the buttons/components in the UI editor do not have to include the page number in their name.

NexText name_t2_profile(3, 3, "t2");
NexButton idscan_b0_profile(3, 5, "b2");
NexButton schedule_b0_profile(3, 1, "b0");

NexVariable chuteid_chuteselc(4, 8, "chuteid");

NexText name_t1_chuteprofile(5, 2, "t1");
NexNumber pills_n0_chuteprofile(5, 4, "n0");
NexButton toAddAlarm_b0_chuteprofile(5, 3, "b0");

NexNumber hour_n0_alarm(6, 4, "n0");
NexNumber min_n1_alarm(6, 5, "n1");
NexNumber dfa_n2_alarm(6, 20, "n2");
NexButton sunday_b0_alarm(6, 6, "b0");
NexButton monday_b8_alarm(6, 14, "b8");
NexButton tuesday_b1_alarm(6, 7, "b1");
NexButton wednesday_b5_alarm(6, 11, "b5");
NexButton thursday_b4_alarm(6, 10, "b4");
NexButton friday_b6_alarm(6, 12, "b6");
NexButton saturday_b7_alarm(6, 13, "b7");
NexButton newTime_b3_alarm(6, 9, "b3");
NexButton finished_b2_alarm(6, 8, "b2");
NexButton weekdays[7] = { sunday_b0_alarm, monday_b8_alarm, tuesday_b1_alarm, wednesday_b5_alarm, thursday_b4_alarm, friday_b6_alarm, saturday_b7_alarm };
String weekday_button_names[7] = { "b0", "b8", "b1", "b5", "b4", "b6", "b7" };

NexText allAlarms_t0_overview(7, 3, "t0");

// Register a button object to the touch event list.  
NexTouch *nex_listen_list[] = {
  &idscan_b0_profile,
  &schedule_b0_profile,
  &toAddAlarm_b0_chuteprofile,
  &newTime_b3_alarm,
  &finished_b2_alarm,
  NULL
};

void idScan_b0_profile_PushCb(void *ptr) {
  // Once user presses the button, goes to a new page
  // interacts with the fingerprint sensor here
  fingerid = 1;
}

Profile tempProfile = Profile();
void schedule_b0_profile_PushCb(void *ptr) {
  // Creates a profile that stores the name and the finger print id
  // into the profile class
  char text_buf[30];
  getText("profile.t2", text_buf, 30);
  tempProfile = Profile(text_buf, fingerid);
}

uint32_t chute_num;
void toAddAlarm_b0_chuteprofile_PushCb(void *ptr) {
  // Store the chute name and number of pills to the appropriate arrays.
  uint32_t int_buf[1];
  char text_buf[30];
  getVal("chuteselc.chuteid", int_buf); // Number of the chute
  chute_num = int_buf[0];
  getText("chuteprofile.t1", text_buf, 30); // Name of the chute
  chute_name_array[chute_num-1] = text_buf;
  getVal("chuteprofile.n0", int_buf); // Number of pills
  pill_array[chute_num-1] = int_buf[0];
  setVal("chuteprofile.n0", 0); // Reset the num of pills on display back to 0
}


void addAlarm() {
  // General add alarm function since newTime and finished buttons 
  // will use this function. 
  uint32_t int_buf[1];
  char text_buf[30];

  Alarm tempAlarm = Alarm();
  getText("profile.t2", text_buf, 30); // Name of the profile

  // Calcuate the number of seconds from the hours and minutes and
  // store it in the alarm class
  getVal("alarm.n0", int_buf); // Hours
  uint32_t seconds = int_buf[0] * 3600;
  getVal("alarm.n1", int_buf); // Minutes
  seconds += int_buf[0] * 60;
  tempAlarm.changeAlarmTime(seconds);
  tempAlarm.changeAlarmChute(chute_num);

  // Store the weekdays to the alarm class
  for (uint8_t ctr = 0; ctr < 7; ctr++) {
    // for all weekdays selected by user, store in alarm class
    String obj_name = "alarm.";
    obj_name = weekday_button_names[ctr];
    getBCo(obj_name, int_buf);
    if (int_buf[0] == GREENCOLOR) {
      // If the weekday is selected, store in alarm class as true
      tempAlarm.setAlarmDay(ctr, true);
      setText("overview.t0", "Test", 4);
    }
  }

  // ALL TESTING BELOW
  // for (uint8_t i = 0; i < 7; i++) {
  //   if(tempAlarm.getAlarmDays(i)) {
  //     // text += 'Y';
  //     setText("overview.t0", "Sus", 3);
  //   }
  // }
  // setText("overview.t0", "ewe", 3);
  // text.toCharArray(text_buf, 30);
  // setText("overview.t0", text_buf, 30);
  // // ?? add alarm to profile class
  // tempProfile.setNewAlarm(tempAlarm);
  // String text; 
  // seconds = tempAlarm.getAlarmTime();
  // uint32_t hours = floor(seconds / 3600);
  // seconds -= hours * 3600;
  // uint32_t min = floor(seconds / 3600);
  
  // utoa(hours, text_buf, 30);
  // text += text_buf;
  // text += ":";
  // utoa(min, text_buf, 30);
  // text += text_buf;
  // // char buf[100];
  // // text_buf = tempProfile.

  // text.toCharArray(text_buf, 30);
}

void newTime_b3_alarm_PushCb(void *ptr) {
  // addAlarm();
  setText("overview.t0", "bruhnext", 30);
}

void finished_b2_alarm_PushCb(void *ptr) {
  addAlarm();
  // Edit allalarms text on the overview page with all alarms
  // Moredays
}


void setup(void) {    
  // Set the baudrate which is for debug and communicate with Nextion screen
  Serial.begin(9600);
    
  // If you have not, you need to change NexConfig.h file in your libary.
  // Instructions are in the file.
  nexInit();

  // Register the pop event callback function of the components
  idscan_b0_profile.attachPush(idScan_b0_profile_PushCb, &idscan_b0_profile);
  schedule_b0_profile.attachPush(schedule_b0_profile_PushCb, &schedule_b0_profile);
  toAddAlarm_b0_chuteprofile.attachPush(toAddAlarm_b0_chuteprofile_PushCb, &toAddAlarm_b0_chuteprofile);
  newTime_b3_alarm.attachPush(newTime_b3_alarm_PushCb, &newTime_b3_alarm);
  finished_b2_alarm.attachPush(finished_b2_alarm_PushCb, &finished_b2_alarm);

  // Set output pins
  // pinMode(motorPin, OUTPUT);

  // Init all the chutes to have 0 pills
  for (int i = 0; i < 6; i++) {
    pill_array[i] = 0;
  }
}

void loop(void) {   
  /*
   * When a pop or push event occurs every time,
   * the corresponding component[right page id and component id] in touch event list will be asked.
   */
//lsjdflkds
  nexLoop(nex_listen_list);
  uint8_t hour[1];
  uint8_t minute[1];
  char day[10];
  if (millis()-prev_millis >= 1000){
    sendCommand("get rtc3");
    recvRetNumber(hour);
    sendCommand("get rtc4");
    recvRetNumber(minute);
  }
  
  if (hour[0] == 0 && current_hour == 23){
    if(current_day <6){
      current_day++;
    }else{
      current_day=0;
    }
  }

  if(minute == 1){
    digitalWrite(3, HIGH);
  }else{
    digitalWrite(3, LOW);
  }
  current_hour = hour[0];
  current_minute = minute[0];

  // TESTING
  // currTime = motorControl(motorPin, currTime);
  // t0p1.setText(ltoa(millis() - currTime, buf, 10));
}



// My own functions for getText, setText, getVal, setVal, getBCo
bool getVal(String obj_name, uint32_t *buffer) {
  // Get the value of obj_name and store it in the buffer
  // Returns if the command was successful
  String cmd = "get ";
  cmd += obj_name;
  cmd += ".val";
  sendCommand(cmd.c_str());
  return recvRetNumber(buffer);
}

bool setVal(String obj_name, uint32_t integer) {
  // Set the value of obj_name to the integer
  // Returns if the command was successful
  char text_buf[30];
  String cmd = obj_name;
  cmd += ".val=";
  utoa(integer, text_buf, 10);
  cmd += text_buf;
  sendCommand(cmd.c_str());
  return recvRetCommandFinished();    
}

uint32_t getText(String obj_name, char *buffer, uint16_t len) {
  // Get the text of obj_name and store it in the buffer
  // Returns the length of the data
  String cmd = "get ";
  cmd += obj_name;
  cmd += ".txt";
  sendCommand(cmd.c_str());
  return recvRetString(buffer, len);
}

uint32_t setText(String obj_name, char *buffer, uint16_t len) {
  // Set the value of obj_name to the text in buffer
  // Returns the length of the data
  String cmd = obj_name;
  cmd += ".txt=\"";
  cmd += buffer;
  cmd += "\"";
  sendCommand(cmd.c_str());
  return recvRetCommandFinished();    
}

uint32_t getBCo(String obj_name, uint32_t *buffer) {
  // Get the background color of obj_name and store it in the buffer
  // Returns the length of the data
  String cmd = "get ";
  cmd += obj_name;
  cmd += ".bco";
  sendCommand(cmd.c_str());
  return recvRetNumber(buffer);
}