///////////////////////////////////////////////
//
// used by setSyncProvider()
//
time_t time_provider()
{
    return rtc.now().unixtime();  
}

/////////////////////////////////////////////////////
//
//
//
long syncTime(void)
{
  int timeOffset = 0;
  long actualTime = 0;
  //Connect to Wifi
  //  WiFi.disconnect();
  //  WiFi.reconnect();
  if(WiFi_Connect_Flag)
  {
    timeClient.begin();
    timeClient.setTimeOffset(timeOffset);
    //  while (!timeClient.update()) {
    timeClient.forceUpdate();
    //  }
    //  timeClient.update();
    actualTime = timeClient.getEpochTime();
    rtc.adjust(DateTime(actualTime));
    Serial.print("Internet Epoch Time: ");
    Serial.println(actualTime);
  }
  DateTime now = rtc.now();
  Serial.print("RTC Epoch Time: ");
  Serial.println(now.unixtime());

  //disconnect WiFi after update
  //  disconnectWiFi();

  return (actualTime);
}

///////////////////////////////////////////////
//
// Function that gets current epoch time
//
unsigned long getTime()
{
  DateTime now = rtc.now();
  Serial.println(now.unixtime());
  return ( now.unixtime());
}

//////////////////////////////////////////////
//
//
//
bool RTC_init(void)
{
  //  Serial.println("Initializing RTC");
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    delay(5000);
    return false;
  }
  Serial.println("Initializing RTC");
  pinMode(RTC_INT_PIN, INPUT_PULLUP);
  rtc.disableAlarm(ALARM_1);      //rtc.alarmInterrupt(ALARM_1, false);
  rtc.disableAlarm(ALARM_2);      //rtc.alarmInterrupt(ALARM_2, false);
  rtc.writeSqwPinMode(DS3231_OFF );    //rtc.squareWave(SQWAVE_NONE);
  rtc.clearAlarm(ALARM_1);
  rtc.clearAlarm(ALARM_2);
  // set alarm 1 to occur every second
  rtc.setAlarm1( dt, DS3231_A1_PerSecond);
  // set alarm 2 to occur every minute
  rtc.setAlarm2( dt, DS3231_A2_PerMinute);

  //  Serial.println("RTC: Waiting to Start....");
  delay(1000);

  DateTime now = rtc.now();
  Serial.print("RTC unixtime = ");
  Serial.println(now.unixtime());

  //if(epochTimeValid) rtc.adjust(epochTime);
  syncTime();
  return true;
}
