#include "IRremote.h"
#include "Adafruit_GFX.h"       // Adafruit graphics library
#include "Adafruit_ST7735.h"    // Adafruit ST7735 display library
#include "SPI.h"                // Arduino SPI library 

using namespace std;

#define TFT_RST  4              //D4 - reset line
#define TFT_CS   8              //D8 - chip select line
#define TFT_DC   7              //D7 - data/command line
#define TFT_SCLK  12
#define MOSI  11

// inverted color definitions
#define BLACK   0xFFFF
#define WHITE   0x0000
#define BLUE    0x07FF
#define RED     0xFFE0 
#define GREEN   0xF81F
#define CYAN    0xFFE0
#define MAGENTA 0x07E0
#define YELLOW  0xF800 
#define ORANGE  0xFE00  
#define POISON  0x68FF
     
int input_pin = 10;   //D10 pin
int output_pin = 9;   //D9 pin
int button = 3;       //D3 pin
int ledB = 13;        //D13 pin

int sAddress = 0, sCommand = 0, sRepeat = 4;
int count = sRepeat;
int led_state = 0, button_state = 1;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, MOSI, TFT_SCLK, TFT_RST);


  void setup()
{
  Serial.begin(9600);
  pinMode(button, INPUT);

  //===PWR BUTTON
  pinMode(5, OUTPUT);  //D5
  digitalWrite(5, HIGH);
  //===PWR BUTTON
  
  pinMode(ledB, OUTPUT);
  IrReceiver.begin(input_pin, ENABLE_LED_FEEDBACK); //Start receiver with feedback
  IrSender.begin(output_pin, ENABLE_LED_FEEDBACK); //Start send with feedback

  tft.initR(INITR_MINI160x80);    // INITR_BLACKTAB  - INITR_MINI160x80  initialize ST7735S TFT display
  tft.setRotation(1);             //Rotação do lcd
  tft.fillScreen(WHITE); 

  Serial.print("Endereço: ");
  Serial.println(sAddress);
  Serial.print("Comando: ");
  Serial.println(sCommand);

  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.print("Endereço: ");
  tft.setTextColor(BLACK);
  tft.println(sAddress);
  tft.setTextColor(BLUE);
  tft.print("Comando: ");
  tft.setTextColor(BLACK);
  tft.println(sCommand);
}


void loop() {
  if (sAddress == 0 && sCommand == 0) {
    delay(20);
    if (IrReceiver.decode()) {
      if (IrReceiver.decodedIRData.command != 0 && IrReceiver.decodedIRData.address !=0) {
        tft.fillScreen(WHITE);      //Clear lcd
        tft.setCursor(0,20);
        
        Serial.print(F("Protocolo decodificado: "));
        Serial.println(getProtocolString(IrReceiver.decodedIRData.protocol));
        tft.setTextColor(BLUE);
        tft.print("Protocolo: ");
        tft.setTextColor(BLACK);
        tft.println(getProtocolString(IrReceiver.decodedIRData.protocol));
    
        Serial.print(F("Dados decodificados: "));
        Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        tft.setTextColor(BLUE);
        tft.print("Dados: ");
        tft.setTextColor(BLACK);
        tft.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    
        Serial.print(F("endereço decodificado: "));
        Serial.println(IrReceiver.decodedIRData.address, HEX);
        sAddress = IrReceiver.decodedIRData.address;
        tft.setTextColor(BLUE);
        tft.print("Endereço: ");
        tft.setTextColor(BLACK);
        tft.println(IrReceiver.decodedIRData.address, HEX);
    
        Serial.print(F("comando decodificado: "));
        Serial.println(IrReceiver.decodedIRData.command, HEX);
        sCommand = IrReceiver.decodedIRData.command;
        tft.setTextColor(BLUE);
        tft.print("Comando: ");
        tft.setTextColor(BLACK);
        tft.println(IrReceiver.decodedIRData.command, HEX);
      }
      IrReceiver.resume();  //Receive next value
    }
  }
  
  if (digitalRead(button) != 1) { 
    led_state = 1; 
    tft.fillScreen(WHITE);      //Clear lcd
    tft.setCursor(0,20);
    }
  
  if (led_state == 1 && sAddress != 0 && sCommand != 0) {
    digitalWrite(ledB, led_state);
    
    
    Serial.print("Enviando >> ");
    Serial.print(sAddress);
    Serial.print(" ");
    Serial.print(sCommand);
    Serial.print(" ");
    Serial.println(sRepeat);
    
    tft.setTextColor(GREEN);
    tft.print("ENVIANDO >> ");
    tft.print(sAddress);
    tft.print(" ");
    tft.print(sCommand);
    tft.print(" ");
    tft.println(sRepeat);

    IrSender.sendNEC(sAddress, sCommand, sRepeat);  //Send IR signal
    
    count = count - 1;
    if (count == 0) {
      sAddress = 0; //Limpa endereço gravado
      sCommand = 0; //Limpa comando gravado
      count = sRepeat;
      digitalWrite(ledB, 0);
      led_state = 0;
      tft.fillScreen(WHITE);      //Clear lcd
    }
  }

}