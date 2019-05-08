


void pixSetup() {
  strip.begin();    // initialize pixel strip
  strip.clear();    // turn all LEDs off
  strip.show();     // update strip

  for (int pix = 0; pix < NUMPIX; pix++) {
    hsi[pix][0] = 200;    //Light Blue
    hsi[pix][1] = 100;
    hsi[pix][2] = 100;
  }
  setupCandle();
  pixDisplay();
  delay(500);
}

void setupCandle() {
  for (int i = 0; i < NUMPIX; i++) {
    
    if (random(0, 100) < 5) {
      //blue/purp
      candleValues[i][0] = random(10, 2000);
      candleValues[i][1] = 230; //Blue
      candleValues[i][2] = 310; //Pink
      candleValues[i][3] = random(100, 2000);
      candleValues[i][4] = random(0, 100);
      candleValues[i][5] = random(150, 255);
    } else {
      //Red/yellow
      candleValues[i][0] = random(50, 3000);
      candleValues[i][1] = 0;
      candleValues[i][2] = random(20,35);
      candleValues[i][3] = random(50, 3000);
      candleValues[i][4] = random(0, 100);
      candleValues[i][5] = random(150, 255);
    }
  }
}

void updateCandle() {
//  if(millis()%10000 == 0){
//    setupCandle();
//  }
  long timer = millis(); // updating time
  
  int off = (int)timer/10000;
  
  for (int i = 0; i < NUMPIX; i++) {
    int idx = (i + off)%NUMPIX;
    int h_period = candleValues[idx][0];
    int h = cosWave( h_period, timer);
    int mapped_h = map(h, 0, 255, candleValues[idx][1], candleValues[idx][2]);
    
    
    int b_period = candleValues[idx][3];
    int b = cosWave( b_period, timer);
    int mapped_b = map(b, 0, 255, candleValues[idx][4], candleValues[idx][5]);
    
    Serial.print(mapped_h);
    Serial.print("\t");
    hsi[i][0] = mapped_h;
    hsi[i][1] = 200;
    hsi[i][2] = mapped_b;
//    hsi[i][2] = 255;
  }
  Serial.println();
  pixDisplay();

}

int cosWave(int period, long timer) {
  return  (int) (sin( millis()/(float)period ) * 127 + 128);
}

void changeHue(int pix, int val) {
  //Limit and wrap to 255
  hsi[pix][0] = ( hsi[pix][0] + val ) % 255;
}

void pixSetHue(int hue) {
  for (int pix = 0; pix < NUMPIX; pix++) {
    hsi[pix][0] = hue;    //Light Blue
  }
}

void pixSetSat(int sat) {
  for (int pix = 0; pix < NUMPIX; pix++) {
    hsi[pix][1] = sat;    //Light Blue
  }
}

void pixSetBri(int bri) {
  for (int pix = 0; pix < NUMPIX; pix++) {
    hsi[pix][2] = bri;    //Light Blue
  }
}

void pixSetHSB(int _h, int _s, int _b) {

  pixSetHue(_h);
  pixSetSat(_s);
  pixSetBri(_b);
  pixDisplay();
}

void pixFadeOut() {
  Serial.println("BEGIN FADE");
  for(int i = 0; i < 255; i++){
    Serial.print(".");
    for (int pix = 0; pix < NUMPIX; pix++){
      if(hsi[pix][0] > 0){
        hsi[pix][0]--;
      }
      if(hsi[pix][1] > 0){
        hsi[pix][1]--;
      }
      if(hsi[pix][2] > 0){
        hsi[pix][2]--;
      }
    }
    pixDisplay();
    delay(2);
  }
  Serial.println();
  Serial.println("END FADE");
}



void pixDisplay() {

  for (int pix = 0; pix < NUMPIX; pix++) {
    RGBColor color = converter.HSItoRGB(hsi[pix][0], hsi[pix][1], hsi[pix][2]);
    // set the color for this pixel:
    strip.setPixelColor(pix, color.red, color.green, color.blue);
  }
  strip.show();   // update the strip
}


