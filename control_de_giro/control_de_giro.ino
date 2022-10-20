#define S1 2    // Control de giro a la derecha PIN2 del Arduino
#define S2 3    // Detencion de giro PIN3 del Arduino
#define S3 4    // Control de giro a la izquierda PIN4 del Arduino
#define In1 10  // PIN2 del L293D
#define In2 9   // PIN7 del L293D 

unsigned int acceleration_ramp = 0, change_twirl = 0, stop = 0;
unsigned int actual_state, old_state;

void spinControl(int pin_stop, int direction, int turn_reversal);
void accelerationStop(int direction);

void setup(){
  pinMode(In1, OUTPUT); 
  pinMode(In2, OUTPUT);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT); 
  pinMode(S3, INPUT); 
  analogWrite(In1, 0);
  analogWrite(In2, 0);
  //Serial.begin(9600);
}
 
void loop(){
  if(!digitalRead(S1) || change_twirl == In2){ //Gira a la derecha
    change_twirl = 0;
    old_state = In1;
    if(actual_state != old_state) spinControl(In2, In1, S3);
  }
  
  else if(!digitalRead(S2) && acceleration_ramp > 0){ //Detiene el motor
    if(change_twirl == In1 || stop == 0) accelerationStop(In2);
    else if(change_twirl == In2 || stop == 1) accelerationStop(In1);
    analogWrite(In1, 0);
    analogWrite(In2, 0);
  }

  else if(!digitalRead(S3) || change_twirl == In1){//Gira a la izquierda
    change_twirl = 0;
    old_state = In2;
    if(actual_state != old_state) spinControl(In1, In2, S1);
  }
}

void spinControl(int pin_stop, int direction, int turn_reversal){
  actual_state = direction;  
  if(acceleration_ramp > 0){ //En caso de inversion de giro, primero desacelera
    accelerationStop(pin_stop);
  }else analogWrite(pin_stop, 0);

  for(int i = 0; i < 256; i++){
    analogWrite(direction, acceleration_ramp);
    delay(50);
    acceleration_ramp++;
    //Serial.print("Aceleracion: ");
    //Serial.println(acceleration_ramp);
    
    if(acceleration_ramp >= 255) acceleration_ramp = 255;
  
    if(!digitalRead(S2)){ //Detiene el motor
      accelerationStop(direction);
      break;
    }

    if(pin_stop == In2) stop = 1;
    else stop = 0;

    if(!digitalRead(turn_reversal)){ //Inversion de giro derecha o izquierda 
      accelerationStop(direction);
      actual_state = pin_stop;
      change_twirl = direction;
      break;
    }
  }
}

void accelerationStop(int direction){ //Desaceleracion progresiva hasta detenerte 
  unsigned int duty = acceleration_ramp;
  for(int i = 0; i < duty; i++){
    analogWrite(direction, acceleration_ramp);
    delay(50);
    acceleration_ramp--;
    //Serial.print("Desaceleracion: ");
    //Serial.println(acceleration_ramp);
    
    if(acceleration_ramp <= 0) acceleration_ramp = 0;
  }analogWrite(direction, 0);
}