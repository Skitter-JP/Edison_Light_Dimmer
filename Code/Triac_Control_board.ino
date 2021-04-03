#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>

unsigned int timerovf = 530;
unsigned int x = timerovf/100;
unsigned int delta = 12;


unsigned DIM[2] = {100,100};
unsigned NEWDIM[2] = {100,100};


#define RL_OFF PORTB  &= (1 << PB5)
#define RL_ON PORTB |= ~(1 << PB5)
#define RL 12

#define LED2_OFF PORTD  &= (1 << PD6)
#define LED2_ON PORTD |= ~(1 << PD6)
#define LED2 6

#define LED1_OFF PORTD  &= (1 << PB5)
#define LED1_ON PORTD |= ~(1 << PB5)
#define LED1 5

void zeroCrossingInterrupt(){ //zero cross detect   
  TCNT1 = 0;   //reset timer - count from zero
}

void timerint(){
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  attachInterrupt(0,zeroCrossingInterrupt, RISING); //Interupt on pin2
  TCCR1A = 0b11110010; //FAST PWM and PORT charateristcs
  TCCR1B = 0b00011100; // Prescalar 256 
  ICR1 = timerovf;
  OCR1B = timerovf;
  OCR1A = timerovf;
  sei();
}

void setup() {

  Serial.begin(9600);
  pinMode(RL,OUTPUT);
  RL_OFF;
  pinMode(LED1,OUTPUT);
  LED1_OFF;
  pinMode(LED2,OUTPUT);
  LED2_OFF;

  attachInterrupt(0,zeroCrossingInterrupt, FALLING);    
  timerint();
    
  Wire.begin(9);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

}

void receiveEvent(int howMany) {

  for (int i = 0; i < howMany; i++)
    {
      
    NEWDIM[i] =Wire.read ();
		if(NEWDIM[i] > 100 || NEWDIM[i] < 0){
		  NEWDIM[i] = 100;
		}
    }  // end of for loop
    

    
    
}

void fade(int startt, int endval, byte gate, int count ){
  for(int i = startt; i <= endval;i++){
    if(gate == 1){
      OCR1A = i;
      DIM[0]= i/x;
    }

    if(gate == 2){
      OCR1B = i;
      DIM[1]= i/x;
    }
    delay(count);
     
  }
 
}

void increase(int startt, int endval, byte gate, int count ){
   for(int i = startt; i >= endval;i--){
      if(gate == 1){
      OCR1A = i;
      DIM[0]= i/x;
      }else
      {
        OCR1B = i;
        DIM[1]= i/x;
      }
        delay(count);
      }
}

void loop() {
  
  if( DIM[0] != NEWDIM[0]){

    if(DIM[0] > NEWDIM[0]){
          
          if(DIM[0] == 100){
            RL_ON;
            LED1_ON;
            delay(5);
          }
      increase(x*DIM[0],x*NEWDIM[0],1,delta);
    }

    
    if(DIM[0] < NEWDIM[0]){
      fade(x*DIM[0],x*NEWDIM[0],1,delta);
      
        if(DIM[0] == 100){
           OCR1A = timerovf;
           delay(5);
           LED1_OFF;
     
        }  
    }
// DIM[0] = NEWDIM[0]; 

 }

if( DIM[1] != NEWDIM[1]){

    if(DIM[1] > NEWDIM[1]){
      
          if(DIM[1] == 100){
            RL_ON;
            LED2_ON;
            delay(5);
          }
      increase(x*DIM[1],x*NEWDIM[1],2,delta);
    }

    
    if(DIM[1] < NEWDIM[1]){
      fade(x*DIM[1],x*NEWDIM[1],2,delta);
      
        if(DIM[1] == 100){
           OCR1B = timerovf;
           delay(5);
           LED2_OFF;
     
        }  
    }
 //DIM[1] = NEWDIM[1]; 

 }
 if(DIM[0] == 100 && DIM[1] == 100){
  RL_OFF;
 }
}
