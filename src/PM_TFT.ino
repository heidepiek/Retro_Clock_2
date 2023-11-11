/*
  Licensed as Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
  You are free to:
  Share — copy and redistribute the material in any medium or format
  Adapt — remix, transform, and build upon the material
  Under the following terms:
  Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
  NonCommercial — You may not use the material for commercial purposes.
  ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original
*/
// #############################################################
//
//      TTTTTTT   OOOOO    U     U    CCCCC    H     H
//         T     O     O   U     U   C     C   H     H
//         T     O     O   U     U   C         HHHHHHH
//         T     O     O   U     U   C     C   H     H
//         T      ooooo     UUUUU     CCCCC    H     H
//
// #############################################################

///////////////////////////////////////
//
//  TFT SPI only
//  Read Touch Screen position (x,y)
//
bool touchCheck(void)
{
  uint16_t x, y;
  // Check to verify previous TFT touch was processed
  //  if it hasn't been processed return true.
  //  if (touchData.touchedFlag)
  //  {
  //    return (true);
  //  }
  // See if there's any new touch data
  if (myGLCD.getTouch(&x, &y))
  {
    touchData.x = x;
    touchData.y = y;
    touchData.row = 0;
    touchData.column = 0;
    if (x <= 100) //  Left Column
    {
      touchData.button = 1;
    }
    else if (x >= 220)  // Right Column
    {
      touchData.button = 3;
    }
    else    // Center column
      touchData.button = 2;
    if (y >= 180)   // Row #4 (bottom)
    {
      touchData.button += 9;
    }
    else if (y >= 120) // Row #3
    {
      touchData.button += 6;
    }
    else if (y >= 60) // Row #2
    {
      touchData.button += 3;
    }
#ifdef DEBUG_TOUCH
    Serial.printf("Button = %d,  x = %d, y = %d\n\r", touchData.button, x, y);
#endif
    touchData.row = (y - pgStart) / pgLineHeight;
    TOUCH_PRINTF("row = %d\n\r", touchData.row);
    //
    // determine what column (button) was touched
    //
    if ( x < col_1_Start )
    {
      touchData.column = 0;
      TOUCH_PRINTLN("Touch column = 0");
    }
    else if ( x < col_2_Start )
    {
      touchData.column = 1;
      TOUCH_PRINTLN("Touch column = 1");
    }
    else if ( x < col_3_Start )
    {
      touchData.column = 2;
      TOUCH_PRINTLN("Touch column = 2");
    }
    else
    {
      touchData.column = 3;
      TOUCH_PRINTLN("Touch column = 3");
    }
    delay(200);
    return (true);
  }
  return (false);
}


//
//
//
void setGfxFont(const GFXfont *font)
{
  gfxFont = font;
  myGLCD.setFreeFont(font);
}

typedef struct ATTR_STRUCT {
  uint8_t textSize;
  uint8_t textFont;
  uint8_t textDatum;
  uint32_t textColor;
  uint32_t textBgColor;
  const GFXfont *freeFont;
} TEXT_ATTR;

#define ATTR_STACK_SIZE 8
TEXT_ATTR *textAttr;
TEXT_ATTR attrStack[ATTR_STACK_SIZE];
int attrStkPtr = 0;

//
//  Push Text Attributes
//
void pushTA(void)
{
  if (attrStkPtr < ATTR_STACK_SIZE)
  {
#ifdef PUSH_POP_DEBUG
    Serial.printf("pre-PUSH %x\n\r", attrStkPtr);

    Serial.printf("textfont = %d\n\r", myGLCD.textfont);
    Serial.printf("textsize = %d\n\r", myGLCD.textsize);
    Serial.printf("textdatum = %d\n\r", myGLCD.textdatum);
    Serial.printf("freeFont = %x\n\r", gfxFont);
#endif
    attrStack[attrStkPtr].textFont = myGLCD.textfont;
    attrStack[attrStkPtr].textSize = myGLCD.textsize;
    attrStack[attrStkPtr].textDatum = myGLCD.textdatum;
    attrStack[attrStkPtr].freeFont = gfxFont;
    attrStack[attrStkPtr].textColor = myGLCD.textcolor;
    attrStack[attrStkPtr].textBgColor = myGLCD.textbgcolor;
    attrStkPtr++;
#ifdef PUSH_POP_DEBUG
    Serial.printf("post-PUSH %x\n\r", attrStkPtr);
#endif
  }
  else
  {
    Serial.println("Attribute stack overflow");
    //sysLog("Attribute stack overflow");
    // Dump Stack
    for (int i = 0; i < ATTR_STACK_SIZE; i++)
    {
      Serial.printf("%d = %d\r\n", i, attrStack[i].textFont);
    }
    delay(2000);
    ESP.restart();
    while (1);
  }
}

//
//  Pop Text Attributes
//
void popTA(void)
{
  if (attrStkPtr > 0)
  {
#ifdef PUSH_POP_DEBUG
    Serial.printf("pre-POP %x\n\r", attrStkPtr);
#endif
    --attrStkPtr;
#ifdef PUSH_POP_DEBUG
    Serial.printf("post-POP %x\n\r", attrStkPtr);
    Serial.printf("textfont = %d\n\r", attrStack[attrStkPtr].textFont);
    Serial.printf("textsize = %d\n\r", attrStack[attrStkPtr].textSize);
    Serial.printf("textdatum = %d\n\r", attrStack[attrStkPtr].textDatum);
    Serial.printf("freeFont = %x\n\r", attrStack[attrStkPtr].freeFont);
#endif
    myGLCD.setTextDatum(attrStack[attrStkPtr].textDatum);
    myGLCD.setTextSize(attrStack[attrStkPtr].textSize);       // Text size multiplier
    myGLCD.setTextFont(attrStack[attrStkPtr].textFont);
    gfxFont = attrStack[attrStkPtr].freeFont;
    myGLCD.setFreeFont(gfxFont);
    //myGLCD.setTextColor(attrStack[attrStkPtr].textColor);
    myGLCD.textcolor = attrStack[attrStkPtr].textColor;
    myGLCD.textbgcolor = attrStack[attrStkPtr].textBgColor;
  }
  else
  {
    Serial.println("Attribute stack underflow");
    //sysLog("Attribute stack underflow");
    // Dump Stack
    for (int i = 0; i < ATTR_STACK_SIZE; i++)
    {
      Serial.printf("%d = %d\r\n", i, attrStack[i].textFont);
    }
    delay(2000);
    ESP.restart();
    while (1);
  }

}

//
//  Draw Button (1 - 12)
//
//    Four ROWS by Three COLUMNS (12 Buttons)
//      Top row (left to right) is Buttons #1 #2 #3
//      Second row (left to right) is Buttons #4 #5 #6
//      Third row (left to right) is Buttons #7 #8 #9
//      Bottom row (left to right) is Buttons #10 #11 #12
//
//      There is no Button #0 (ZERO)
//
#define B_RADIUS 10
#define B_HEIGHT 45
#define B_WIDTH 90
//
//    Location of text within Button (x and y axis)
//
int xPosTblText[3] = {55, 160, 265};        // index = (buttonNbr - 1) % 3
int yPosTblText1[4] = {31, 89, 149, 207};   // index = (buttonNbr - 1) / 3
int yPosTblText2[4] = {22, 80, 140, 198};   // index = (buttonNbr - 1) / 3
//
//    Draw Button
//        Input:  buttonNbr = Button Number
//                color = Color of Button Text
//                text = Button Text (first line)
//                text2 = Second of Button Text (if not blank, "")
//
void drawBtn(int buttonNbr, int color, String text, String text2)
{
  int yPos1 = 0;
  int yPos2 = 0;
  //  First validate Button Number
  if ((buttonNbr < 1) || (buttonNbr > 12)) return;
  // Push Text Attributes on attribute stack
  pushTA();
  int xPos = xPosTblText[(buttonNbr - 1) % 3];
  setGfxFont(FSS9);
  myGLCD.setTextSize(1);          // Text size multiplier
  myGLCD.setTextColor(color, TFT_BLACK);
  myGLCD.setTextDatum(MC_DATUM);         // Middle-Center Text
  //  Determine if Button contains one or two lines of text
  if (text2.length() == 0)  // if one line of text use table #1
  {
    yPos1 = yPosTblText1[(buttonNbr - 1) / 3];
  }
  else  // if two lines of text use table #2 for first line
  {
    yPos1 = yPosTblText2[(buttonNbr - 1) / 3];
    yPos2 = yPos1 + 16;             // Second line of text is at +16
  }
  //  Draw blank Button
  switch (buttonNbr)
  {
    case 1:
      myGLCD.fillRoundRect(10 , 10 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(10 , 10 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #1
      break;
    case 2:
      myGLCD.fillRoundRect(115 , 10 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(115 , 10 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW);  // Button #2
      break;
    case 3:
      myGLCD.fillRoundRect(220 , 10 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(220 , 10 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW);  // Button #3
      break;
    case 4:
      myGLCD.fillRoundRect(10 , 68 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(10 , 68 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #4
      break;
    case 5:
      myGLCD.fillRoundRect(115 , 68 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(115 , 68 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #5
      break;
    case 6:
      myGLCD.fillRoundRect(220 , 68 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(220 , 68 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #6
      break;
    case 7:
      myGLCD.fillRoundRect(10 , 127 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(10 , 127 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #7
      break;
    case 8:
      myGLCD.fillRoundRect(115 , 127 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(115 , 127 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #8
      break;
    case 9:
      myGLCD.fillRoundRect(220 , 127 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(220 , 127 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #9
      break;
    case 10:
      myGLCD.fillRoundRect(10 , 185 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(10 , 185 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #10
      break;
    case 11:
      myGLCD.fillRoundRect(115 , 185 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(115 , 185 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #11
      break;
    case 12:
      myGLCD.fillRoundRect(220 , 185 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_BLACK);
      myGLCD.drawRoundRect(220 , 185 , B_WIDTH  , B_HEIGHT  , B_RADIUS , TFT_YELLOW); // Button #12
      break;
    default:
      break;
  }
  // Put text into Button
  myGLCD.drawString(text, xPos, yPos1);
  if (yPos2) myGLCD.drawString(text2, xPos, yPos2);
  // Pop Text Attributes from attribute stack
  popTA();
}

////////////////////////////////////////////////////////////
//
//  Draw switch on vertical axis (uses two buttons worth of screen)
//      must specify top button (can't be buttons 10, 11 or 12)
//      switch position is ON = true, OFF = false (ON is on top, OFF is bottom)
//
void drawSwitchV(short button, boolean swPosition)
{
  int xpos = buttonXpos[button];
  int ypos = buttonYpos[button];
  //Serial.printf("button = %d, xpos = %x, ypos = %x, ON/OFF = %d\n\r", button, xpos, ypos, swPosition);
  drawVertSwitch(xpos, ypos, swPosition);
}

//
//  Draw Vertical Switch
//
void drawVertSwitch(int xpos, int ypos, boolean swPosition)
{
  pushTA();
  setGfxFont(FSS9);
  myGLCD.setTextSize(1);          // Text size multiplier
  myGLCD.setTextColor(TFT_WHITE, TFT_BLACK);
  myGLCD.setTextDatum(MC_DATUM);         // Middle-Center Text 
  if (swPosition == true)
  {
    myGLCD.drawString("ON", xpos + 15, ypos + 2);
    myGLCD.fillRoundRect(xpos, ypos + 15, 31, 60, B_RADIUS , TFT_GREEN);    //w=90, h=45
    myGLCD.drawRoundRect(xpos, ypos + 15, 31, 60, B_RADIUS , TFT_WHITE);
    myGLCD.fillCircle(xpos + 15, ypos + 28, 10, TFT_DARKGREY);
    myGLCD.drawCircle(xpos + 15, ypos + 28, 10, TFT_WHITE);
  }
  else
  {
    myGLCD.fillRoundRect(xpos, ypos + 15, 31, 60, B_RADIUS , TFT_RED);
    myGLCD.drawRoundRect(xpos, ypos + 15, 31, 60, B_RADIUS , TFT_WHITE); // Button #8   
    myGLCD.fillCircle(xpos + 15, ypos + 61, 10, TFT_DARKGREY);
    myGLCD.drawCircle(xpos + 15, ypos + 61, 10, TFT_WHITE);
    myGLCD.drawString("OFF", xpos + 15, ypos + 87);
  }
  popTA();
}

////////////////////////////////////////////////////////////
//
//  Draw switch on horizontal axis (uses two buttons worth of screen)
//      must specify left button (can't be buttons 3, 6, 9, or 12)
//      switch position is ON = true, OFF = false (ON --> right, OFF <-- left)
//
void drawSwitchH(short button, boolean swPosition)
{
  int xpos = buttonXpos[button];
  int ypos = buttonYpos[button];
  //Serial.printf("button = %d, xpos = %x, ypos = %x, ON/OFF = %d\n\r", button, xpos, ypos, swPosition);
  drawHorizSwitch(xpos, ypos, swPosition);
}

//
//  Draw Horizontal Switch
//
void drawHorizSwitch(int xpos, int ypos, boolean swPosition)
{
  pushTA();
  setGfxFont(FSS9);
  myGLCD.setTextSize(1);          // Text size multiplier
  myGLCD.setTextColor(TFT_WHITE, TFT_BLACK);
  myGLCD.setTextDatum(MC_DATUM);         // Middle-Center Text
  if (swPosition == true)
  {
    myGLCD.drawString("ON", xpos + 110, ypos + 20);
    myGLCD.fillRoundRect(xpos, ypos, 90, 40, B_RADIUS , TFT_GREEN);    //w=90, h=45
    myGLCD.drawRoundRect(xpos, ypos, 90, 40, B_RADIUS , TFT_WHITE);
    myGLCD.fillRoundRect(xpos + 55, ypos + 3, 30, 34, B_RADIUS , TFT_DARKGREY); 
    myGLCD.drawRoundRect(xpos + 55, ypos + 3, 30, 34, B_RADIUS , TFT_WHITE);
  }
  else
  {
    myGLCD.fillRoundRect(xpos, ypos, 90, 40, B_RADIUS , TFT_RED);
    myGLCD.drawRoundRect(xpos, ypos, 90, 40, B_RADIUS , TFT_WHITE); // Button #8 
    myGLCD.fillRoundRect(xpos + 5, ypos + 3, 30, 34, B_RADIUS , TFT_DARKGREY); 
    myGLCD.drawRoundRect(xpos + 5, ypos + 3, 30, 34, B_RADIUS , TFT_WHITE);  
    myGLCD.drawString("OFF", xpos - 25, ypos + 18);
  }
  popTA();
}

//////////////////////////////////////////////////
//
//
//
void drawVolumeLevel(int level, bool text)
{
  pushTA();
  //setGfxFont(FSS9);
  myGLCD.setTextSize(1);          // Text size multiplier
  myGLCD.setTextColor(TFT_WHITE, TFT_BLACK);
  myGLCD.setTextDatum(MC_DATUM);         // Middle-Center Text
  if(level > 100) level = 100;
  if(level < 0) level = 0;
  myGLCD.fillRect(120, 129, 25, 100, TFT_BLUE);    //w=90, h=45
  myGLCD.fillRect(120, 129, 25, 100-level, TFT_BLACK);
  myGLCD.drawRect(120, 129, 25, 100, TFT_WHITE);
  if(text)
  {
    int yPos = 129 + (100 - level);
    if( yPos > 223) yPos = 223;
    myGLCD.drawNumber(level, 132, yPos, 1);     // xPos was 160
  }
  popTA();
}


void drawSettingsBtn(int buttonNbr)
{
  int xPos = COL2XPOS;    // Button 5
  int yPos = ROW2YPOS;    // Button 5
    //  First validate Button Number
  if ((buttonNbr < 1) || (buttonNbr > 12)) return;
  // Push Text Attributes on attribute stack
  pushTA();
  xPos = xPosTblText[(buttonNbr - 1) % 3];
  yPos = yPosTblText1[(buttonNbr - 1) / 3];
  setGfxFont(FSS9);
  myGLCD.setTextSize(1);          // Text size multiplier
  //myGLCD.setTextColor(color, TFT_BLACK);
  myGLCD.setTextDatum(MC_DATUM);         // Middle-Center Text
  
  myGLCD.fillCircle(xPos, yPos, 10, TFT_WHITE);

  
  popTA();
}

//
//
//  Touch Calibrate, TFT SPI only
//
//
void touch_calibrate()
{
  uint16_t calData[5];
  bool calDataOK = false;

  // check file system exists

  //  tftLedOn();
  // Open dir folder
  File dir = SPIFFS.open("/");
  // List file at root
  //  listFilesInDir(dir, 1);


  // check if calibration file exists and size is correct
  //
  if (existsFile(fnTftCal))
  {
    if (rdFile(fnTftCal, (char *)calData, 14))
      calDataOK = true;
    TOUCH_PRINTLN(F("\r\n=== TFT Touch Calibration Data ==="));
    for (int z = 0; z < 5; z++)
    {
      TOUCH_PRINT("\t");
      TOUCH_PRINTLN(calData[z]);
    }
  }
  else calibrateTftTouch = true;
  if (calDataOK && !calibrateTftTouch)
  {
    // calibration data valid
    myGLCD.setTouch(calData);
  }
  else
  {
    // data not valid, calibrate TFT Touch
    myGLCD.fillScreen(TFT_BLACK);
    myGLCD.setCursor(20, 0);
    myGLCD.setTextFont(2);
    myGLCD.setTextSize(1);          // Text size multiplier
    myGLCD.setTextColor(TFT_WHITE, TFT_BLACK);
    myGLCD.println("Touch corners as indicated");
    myGLCD.setTextFont(1);
    myGLCD.println();
    myGLCD.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
    for (int d = 0 ; d < 5; d++)
    {
      TOUCH_PRINTLN(calData[d]);
    }
    // Save TFT Touch calibration data
    wrtFile(fnTftCal, (char *)calData, 14);
    calibrateTftTouch = false;
    myGLCD.fillScreen(TFT_BLACK);
    myGLCD.setTextSize(3);          // Text size multiplier
    myGLCD.setTextColor(TFT_GREEN, TFT_BLACK);
    myGLCD.setTextDatum(TC_DATUM);         // Center Text
    myGLCD.drawString("Calibration", 160, 90);
    myGLCD.drawString("complete!", 160, 120);
    delay(2000);
  }
}

//
//
//
void ldrInit(void)
{
  //deleteFile(fnLdrConfig);
  if (!existsFile(fnLdrConfig))
  {
    Serial.println("Creating LDR configuration file");
    LdrConfig.cal = false;
    // dark = 3200, light = 1000 with blue decal
    LdrConfig.daylight = 1200;    //100;
    LdrConfig.night = 0;
    LdrConfig.step = 450;
    wrtFile(fnLdrConfig, (char *)&LdrConfig, sizeof(LdrConfig));
  }

  Serial.println("Reading LDR configuration file");
  if (rdFile(fnLdrConfig, (char *)&LdrConfig, sizeof(LdrConfig)))
  {
#if 1
    Serial.println("\r\n=== LDR Configuration ===");
    Serial.print("\t daylight = ");
    Serial.println(LdrConfig.daylight);
    Serial.print("\t step = ");
    Serial.println(LdrConfig.step);
#endif
  }
}

void ldrCalibration(void)
{
  switch(ldrCalState)
  {
    case 0:
      // read LDR ADC value   // dark ~ 4095,  light ~ 1400 // dark 3200, light 1000
      LdrConfig.daylight = analogRead(LDR_PIN);
      LdrConfig.step = (4000 - LdrConfig.daylight) / 5;
      wrtFile(fnLdrConfig, (char *)&LdrConfig, sizeof(LdrConfig)); 
      Serial.printf("daylight = %d, step = %d\n\r", LdrConfig.daylight, LdrConfig.step);
      ldrCalState++;
      break;
    case 1:
      ledcWrite(LED_PWM_CHANNEL, 255);
      ldrCalState++;
      break;
    case 2:
      ledcWrite(LED_PWM_CHANNEL, 200);
      ldrCalState++;
      break;
    case 3:
      ledcWrite(LED_PWM_CHANNEL, 150);
      ldrCalState++;
      break;
    case 4:
      ledcWrite(LED_PWM_CHANNEL, 100);
      ldrCalState++;
      break;
    case 5:
      ledcWrite(LED_PWM_CHANNEL, 50);
      ldrCalState++;
      break;
    case 6:
      ldrCalState = 0;
      ldrCalibrationFlag = false;
      break;
    default:
      ldrCalibrationFlag = false;
      break;
  }
}
