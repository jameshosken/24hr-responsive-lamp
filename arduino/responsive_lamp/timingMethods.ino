
int getHour()
{ 
  //No need for GMT?
  Serial.println("#########");
  Serial.println("GET HOURS(): ");
  
  int hour = ((rtc.getHours() + GMT) % 24);
  Serial.print("RAW: ");
  Serial.println(rtc.getHours());
  if(hour < 0){
    hour += 24;
  }
  
  Serial.print("CONVERTED: ");
  Serial.println(hour);
  
  delay(10);
  return (hour);
}


void handleTimingColours(int hour) {
  if(hour == -1){
    Serial.println("Wrong Hour");
    return;
  }
//  //Hack since modulo doesn't seem to be working...
//  if(hour < 0){
//    hour += 24;
//  }
  
  Serial.println("\n\n\n>>>>>>>>>NEW TIME");
  Serial.print("HOUR: ");
  Serial.println(hour);
  //Midnight to midnight
  /*
     Night: reddish orange - 40,50,10
     Sunrise:
        Red - 0, 50, 25
        Orange - 30, 60, 50
        Yellow - 50, 75, 75
        Blue - 200, 25, 100

     Daytime: Off
     Sunset:
      Blue - 200, 25, 100
      Purple - 280, 75, 75
      Pink - 320, 50, 75
      Deep red: 0, 75, 50

     Evening: Warm white - 22, 50, 100

  */
  int hueColours[][3] = {
    HSB_NIGHT,      //12-1
    HSB_NIGHT,      //1-2
    HSB_NIGHT,      //2-3
    HSB_NIGHT,      //3-4
    HSB_NIGHT,      //4-5
    HSB_PALE_RED,   //5-6
    HSB_ORANGE,     //6-7
    HSB_YELLOW,     //7-8
    HSB_BLUE,       //8-9
    HSB_OFF,        //9-10
    HSB_OFF,        //10-11
    HSB_OFF,        //11-12
    HSB_OFF,        //12-1
    HSB_OFF,        //1-2
    HSB_OFF,        //2-3
    HSB_OFF,        //3-4
    HSB_ORANGE,     //4-5
    HSB_PURPLE,     //5-6
    HSB_PINK,       //6-7
    HSB_DEEP_RED,   //7-8
    HSB_WW,         //8-9
    HSB_WW,         //9-10
    HSB_WW,         //10-11
    HSB_NIGHT       //11-12    
  };

  //Cathc off state
  if(hour >=9 && hour <= 15){
    sendRequest(bulb, "on", "false");
    Serial.println("Turning light off");
    return;
  }else{
    sendRequest(bulb, "on", "true");
    Serial.println("Turning light on");
    delay(1000);
  }
  
  //Use hour as index to get colour for current hour
  int current[] = {hueColours[hour][0], hueColours[hour][1], hueColours[hour][2]};
  Serial.print(current[0]);
  Serial.print(", ");
  Serial.print(current[1]);
  Serial.print(", ");
  Serial.println(current[2]);
  
  setPhilipsHSB(current[0], current[1], current[2]);
  Serial.println("<<<<<<<<<<<<<<<<<<<<<\n\n\n");
}



