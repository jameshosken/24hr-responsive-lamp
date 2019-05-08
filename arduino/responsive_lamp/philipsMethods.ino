int hueToPhilips(int hue){
  return (int) constrain( ((float)hue * 182.04), 0, 65534); // map 360 to 65535
}

int satToPhilips(int sat){
  return (int) constrain( ((float)sat * 2.54), 0, 254); // map 360 to 65535
}

int briToPhilips(int bri){
  return (int) constrain( ((float)bri * 2.54), 0, 254); // map 360 to 65535
}


void setPhilipsHSB(int _h, int _s, int _b){
  Serial.println("\n\n");
  Serial.println("Setting Philips");
  //Bri first, then sat, then hue, to avoid sudden flashes of unwanted bright colours
  int b = briToPhilips(_b);
  sendRequest(bulb, "bri", String(b));   // turn light on

  int h = hueToPhilips(_h);
  sendRequest(bulb, "hue", String(h));   // turn light on

  int s = satToPhilips(_s);
  sendRequest(bulb, "sat", String(s));   // turn light on
  Serial.println("Set Philips");
  Serial.println("\n\n");
  
}

