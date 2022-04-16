// 4/9/22 Tyler Chan

// Include libraries
#include "Nextion.h"
#include "profile.h"
#include "alarm.h"

char weekday_abbrv[7] = {'U', 'M', 'T', 'W', 'R', 'F', 'S'};
uint8_t fingerid = 0;

// Arrays to store chute information
String chute_name_array[6];
uint8_t pill_array[6];

// Keeping Track of Time

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
NexButton newTime_b3_alarm(6, 9, "b3");
NexButton finished_b2_alarm(6, 8, "b2");
String weekday_variable_names[7] = { "va0", "va8", "va1", "va5", "va4", "va6", "va7" };

NexButton status_b1_idle(9, 13, "b1");

// Register a button object to the touch event list.  
NexTouch *nex_listen_list[] = {
  &idscan_b0_profile,
  &schedule_b0_profile,
  &toAddAlarm_b0_chuteprofile,
  &newTime_b3_alarm,
  &finished_b2_alarm,
  &status_b1_idle,
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
  uint32_t mins = int_buf[0] * 60;
  getVal("alarm.n1", int_buf); // Minutes
  mins += int_buf[0];
  tempAlarm.changeAlarmTime(mins);
  tempAlarm.changeAlarmChute(chute_num);

  // Store the weekdays to the alarm class
  for (uint8_t ctr = 0; ctr < 7; ctr++) {
    // for all weekdays selected by user, store in alarm class
    String obj_name = "alarm.";
    obj_name += weekday_variable_names[ctr];
    getVal(obj_name, int_buf);
    if (int_buf[0] == 1) {
      // If the weekday is selected, store in alarm class as true
      tempAlarm.setAlarmDay(ctr, true);
    } else {
      // If the weekday is not selected, store in alarm class as false
      tempAlarm.setAlarmDay(ctr, false);
    }
  }
  tempProfile.setNewAlarm(tempAlarm);
}

void newTime_b3_alarm_PushCb(void *ptr) {
  addAlarm();
}

void finished_b2_alarm_PushCb(void *ptr) {
  addAlarm();
  // Edit allalarms text on the overview page with all alarms
  char text_buf[100];
  String text = create_upcoming_alarms_text();
  text.toCharArray(text_buf, 100);
  setText("overview.t0", text_buf, 100);
}

void status_b1_idle_PushCb(void *ptr) {
  // If the user presses the status button, go to the status page
  // and display the upcoming alarms
  char text_buf[100];
  String text = create_upcoming_alarms_text();
  text.toCharArray(text_buf, 100);
  setText("idle.t0", text_buf, 100);
}

String create_upcoming_alarms_text() {
  char text_buf[30];
  String text; 
  for (uint8_t i = 0; i < 10; i++) {
    Alarm tempAlarm = tempProfile.getAlarm(i);
    if (tempAlarm.getChuteNo() == 0) {
      // If the alarm is not set, break the loop 
      break;
    }

    // Parse seconds and add "<hr>:<min>" to the text
    uint32_t mins = tempAlarm.getAlarmTime();
    uint32_t hours = floor(mins / 60);
    mins -= hours * 60;
    utoa(hours, text_buf, 10);
    text += text_buf;
    text += ":";
    utoa(mins, text_buf, 10);
    text += text_buf;

    // Parse the weekdays the alarm is set for and add to the text
    for (uint8_t j = 0; j < 7; j++) {
      if (tempAlarm.getAlarmDays(j)) {
        text += " ";
        text += weekday_abbrv[j];
      }
    }
    text += '\r';
  }
  return text;
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
  status_b1_idle.attachPush(status_b1_idle_PushCb, &status_b1_idle);

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

  nexLoop(nex_listen_list);

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