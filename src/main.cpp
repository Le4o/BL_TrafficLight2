#include <mbed.h>

#define RED 1
#define YELLOW 2
#define GREEN 3
#define OFF 0
#define ON 1
#define ALERT 15 
#define START 10

DigitalOut lRed(p6);
DigitalOut lYellow(p7);
DigitalOut lGreen(p8);

InterruptIn btn(p5);

Timeout t;
Ticker ti;

int pState, cState, count, sBtn;

void colorSwitch(int cl);
void stateSwitch();
void attachYellowAlert();
void startCount();
void endCount();
void changeStateToAlert();
void changeStateToOff();
void changeStateToDefault();
void increaseCounter();

void colorSwitch(int cl) {
  switch(cl) { 
    case RED:
      lRed = 1;
      pState = cState;
      cState = RED;
      t.attach(&stateSwitch, 10); 
    case YELLOW:
      lYellow = 1;
      pState = cState;
      cState = YELLO;
      t.attach(&stateSwitch, 4);
    case GREEN:
      lGreen = 1;
      pState = cState;
      cState = GREEN;
      t.attach(&stateSwitch, 20);
  }
}

void stateSwitch() {
  stateToDefault();

  if(cState == START && pState == START) {
    lRed = 1;
    pState = RED;
    cState = RED;
    t.attach(&stateSwitch, 10);
  }
  else if(sBtn == OFF) colorSwitch(cState);
  else if(sBtn == ON) {
      sBtn = OFF;

      if(cState == RED) {
        if(count >= 3 && count <= 10) changeStateToAlert();
        else if(count > 10) changeStateToOff();
        else colorSwitch(RED);
      }
      else if(cState == YELLOW) {
        if(count>=3 && count<=10) changeStateToAlert();
        else if(count > 10) changeStateToOff();
        else colorSwitch(YELLOW);
      }
      else if(cState == GREEN) {
        if(count >= 3 && count <= 10) changeStateToAlert();
        else if(count == 20 || count <= 3) colorSwitch(YELLOW);
        else if(count > 10) changeStateToOff();
        else colorSwitch(GREEN);
      }
      else if(cState == ALERT) {
          if(count >= 3 && count <= 10) {
            if(pState == RED) colorSwitch(RED);
            else if(pState == YELLOW) colorSwitch(YELLOW);
            else colorSwitch(GREEN);
          }
          else if(count > 10) changeStateToOff();
          else {
            cState = pState;
            changeStateToAlert();
          }
      }
      else if(cState == OFF){
          if(count > 10) colorSwitch(RED);
          else changeStateToOff();
      }
  }
}

void attachYellowAlert() {
  lYellow = !lYellow;
  ti.attach(attachYellowAlert, 0.5);
}

void startCount() {
  t.detach();
  count = 0;
  ti.attach(increaseCounter, 1);
}

void endCount() {
  ti.detach();
  sBtn = ON;
  stateSwitch();
}

void changeStateToAlert() {
  pState = cState;
  cState = ALERT;
  attachYellowAlert();
}

void changeStateToOff() {
  pState = cState;
  cState = OFF;
  changeStateToDefault();
}

void changeStateToDefault() {
  lRed = OFF;
  lYellow = OFF;
  lGreen = OFF;
  t.detach();
  ti.detach();
}

void increaseCounter() {
  count++;
}


int main() {

  cState = START;
  pState = START;
  stateSwitch();

  btn.rise(&startCount);
  btn.fall(&endCount);

  while(1) {
    wait(1);
  }
}