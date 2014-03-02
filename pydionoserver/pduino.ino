#include <Servo.h>
#include "Tlc5940.h"
#include "tlc_fades.h"
#include <math.h>


int incomingByte = 0; // for incoming serial data
int lastled = 0; // for incoming serial data
const int LED_NUM = 25;


void setup() {
  Serial.begin(115200);
  Tlc.init();
//  tlc_initServos();
}


int leds[LED_NUM][3]={
  {2,1,0},
  {3,4,5},
  {6,7,8},
  {11,10,9},
  {13,14,15},
  
  {16,17,18},
  {19,20,21},
  {24,23,22},
  {25,26,27},
  {31,30,29},
  
  {34,33,32},
  {37,36,35},
  {40,39,38},
  {41,42,43},  
  {47,45,44},   
  
  {48,49,50}, // new
  {53,52,51},
  {54,55,56},
  {57,58,59},
  {62,61,60},
  
  {64,65,66}, // new
  {67,68,69},
  {70,71,72},
  {73,74,75},
  {78,77,76},


  
};


int ledncp(int ledn, int c, int p){
   Tlc.set(leds[ledn][c], p);
}

double colors[12][3] = {{1,0,0.2}, {1,0,0.4}, {1,0,0.8}, {1,0.2,0}, {1,0.4,0}, {1,0.8,0}, {0,1,0.2}, {0,1,0.4}, {0,1,0.8}, {0,0.2,1},{0,0.4,1}};

int ledcpc(int ledn, int p, double color[]){
  for (int i = 0; i < 3; i++) {
      Tlc.set(leds[ledn][i], p * color[i]);
  }
}

int gledncp(int ledn, int c, int p){
  ledncp(ledn, c, p);
  ledncp((ledn + 2) % LED_NUM, c, p);
  ledncp((ledn + 4) % LED_NUM, c, p);
}

char messageBuffer[2];

int index = 0;
int mode = 0;
int ignore = 0;
int led1 = 0; 
int led2 = 0; 
int led3 = 0; 
int p = 0;
int v = 0;
int lastv = 0;
int rv = 0;
int del = 0;
int minv = -1;
int statmin = -1;
int maxv = -1;
int shifter = 0;
double* thecolor = colors[0];

void turnmeon(int shift, int p, double color[]) {
  for (int i = 0; i < LED_NUM; i+=3) {
     ledcpc((i + shift + random(2)) % LED_NUM, min(p*p, 4000), color);
  }
}

int servoPin = 2;     // Control pin for servo motor
int minPulse = 1200;   // Minimum servo position
int maxPulse = 2100;  // Maximum servo position
int pulse = 0;        // Amount to pulse the servo

int refreshTime = 20; // the time needed in between pulses

int analogValue = 0;  // the value returned from the analog sensor
int analogPin = 0;    // the analog pin that the sensor's on

void doservo(int p){
   digitalWrite(servoPin, HIGH);   // Turn the motor on
    delayMicroseconds(p);       // Length of the pulse sets the motor position
    digitalWrite(servoPin, LOW);    // Turn the motor off
    delay(refreshTime);
}

void loop() {
 
del++;

  while(Serial.available() > 0) {
    char x = Serial.read();
    if (ignore == 1 && x != '!') {
      continue;
    }
    
    if (x == '.') {
      ignore = 1;
      continue;
    }
    else if (x == 'V' || x == 'P'){
      mode = (int)x;
//      Serial.write(1);
//      Serial.write(mode);
    }
    else if (x == '!'){ // end of message
       ignore = 0;
       messageBuffer[index++] = '\0';
//       Serial.write(2);
       if (mode == (int)'V') {
//         Serial.write(3);
         v = atoi(messageBuffer);
//         Serial.write(v);
       }
       else if (mode == (int)'P') {
//         Serial.write(4);
         p = atoi(messageBuffer);
//         Serial.write(p);
       }
       else {
//          Serial.write(9);
//          Serial.write(mode);
       } 
       index = 0;
    }
    else {
/*      Serial.write(5);
      Serial.write(index);
      Serial.write(x);*/
      messageBuffer[index++] = x;
    }
  }
  
   Serial.write(p);
   Serial.write(v);
   Serial.write(0);

   led1 = random(LED_NUM - 3);
      
   if (v > 0) {
    ledcpc(led1, v * v, colors[p % (LED_NUM - 3)]);
    ledcpc(led1 + 1, v * v, colors[p % (LED_NUM - 3)]);
    ledcpc(led1 + 2, v * v, colors[p % (LED_NUM - 3)]);
    ledcpc(led1 + 3, v * v, colors[p % (LED_NUM - 3)]);
   }
  else if (v <= 0) {
      Tlc.set(random(LED_NUM * 3 + 4),0);          
  }
  Tlc.update();
  
  
return;
   
      led1 = random(5) * 3;
      led2 = random(5) * 3 + 1;
      led3 = random(5) * 3 + 2;
      
      //forget
      if (del % 20000 == 0) {
//         Serial.write(1);
//        Serial.write(minv);
//        Serial.write(maxv);
        if (statmin == -1) {
          statmin = minv;
        }
        minv = -1;
        maxv = -1;
      /*  for (int i = 0; i < 15; i++) {
          ledncp(i, 0, 0);
          ledncp(i, 1, 0);          
          ledncp(i, 2, 0);
        }     */
      }
      int thecolorn = random(4);   
      if (minv != -1 && maxv != -1) {
         rv = v - ((minv + maxv) / 2);
      }
      if (v < minv || minv == -1) {
        minv = v;
//        Serial.write(2);
//        Serial.write(minv);

           thecolor = colors[thecolorn];
      }
      
      if (v > maxv   || maxv == -1) {
        maxv = v;
           thecolor = colors[random(4)];       
//        Serial.write(3);
//        Serial.write(maxv);
      }

      // upper leds. more sensitive
       // if (v > 0) {
              if (del % 1000 == 0) {
              minPulse = minPulse * -1;         
              }
            if (v > 10) {
              shifter = random(3);
              turnmeon(shifter % 3, v + 10, colors[0]);
              doservo(minPulse);            
            }
            
            if (v > 8 & v <= 10) {
              turnmeon((shifter + 1) % 3, v + LED_NUM, colors[1]);
            }

            if (v <= 8 && v > 0) {
              turnmeon((shifter + 2) % 3, v + 8, colors[2]);
//              ledcpc(, min(v*v,4000)  , colors[shifter % 3]);
            }
            
            if (v <= 0) {
              Tlc.set(random(LED_NUM * 3 + 4),0);
            }


//           ledcpc((led2 + 1) % 15, min(v * v, 4000), colors[p]);

//        ledcpc(led1, min(v * v, 4000), colors[p]);
/*      
           ledcpc((led1 + 2) % 15, min(v * v, 4000), colors[p]);
           ledcpc((led1 + 3) % 15, min(v * v, 4000), colors[p]);
           ledcpc((led1 + 4) % 15, min(v * v, 4000), colors[p]);
           ledcpc(led1, min(v * v, 4000), colors[p]);*/
/*          if (p > 1) {
           ledcpc((p - 1) % 15, min(v * v * 1.5, 4000), colors[(thecolorn + 1) % 4]);
           ledcpc((p - 0) % 15, min(v * v * 1.5, 4000), colors[(thecolorn + 1) % 4]);
           ledcpc((p + 1) % 15, min(v * v * 1.5, 4000), colors[(thecolorn + 1) % 4]);
          }*/

          lastv = v;

      //  }

//        else {
//           for (int i = 0; i < 48; i ++) {
//            Tlc.set(i,0);
//           }
//        }
        
 
 
  Tlc.update(); 
}
