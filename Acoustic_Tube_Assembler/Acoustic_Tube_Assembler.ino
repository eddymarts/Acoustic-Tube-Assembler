                  /* Industrial Components Assemblies Caribe, SRL
 *  Program for Acoustic Tube Assembler.
 *  
 *  Use Serial Monitor to control 115200 baud
 * 
 * Programmed by Eddy Martínez */
 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class Input {
  private:
    byte pin;
  
  public:
    Input(byte pin) {
      this->pin = pin;

      init();
    }

    void init() {
      pinMode(pin, INPUT);
    }

    bool detect() {
      return digitalRead(pin);
    }

    bool sense() {
      return analogRead(pin);
    }
};

class Output {
  private:
    byte pin;
  
  public:
    Output(byte pin) {
      this->pin = pin;

      init();
    }
    
    void init() {
      pinMode(pin, OUTPUT);
    }

    void d_set(bool value) {
      digitalWrite(pin, value);
    }

    void a_set(int value) {
      analogWrite(pin, value);
    }
};

class PressureSensor : public Input {
  public:
    PressureSensor(byte pin) : Input(pin) {}

    int pressure() {
      return (sense()-99.78)*100/(640.13-99.78);
    }
};

class CylinderSensor : public Input {
  public:
    CylinderSensor(byte pin) : Input(pin) {}

    bool is_activated() {
      bool value = detect();
      Serial.print("\nCindro Horizontal: ");
      if(value) Serial.print("ACTIVADO");
      else Serial.print("DESACTIVADO");
      return value;
    }
};

class Button : public Input {
  private:
    bool button[2]={0,0}; //Last and present value of buttons
  
  public:
    Button(byte pin) : Input(pin) {}

    bool is_pressed() {
      bool result=0;
      button[1] = button[0];
      button[0] = detect();

      return button[1]==1 && button[0]==0;
    }
};

class Valve : public Output {
  private:
    char name;

  public:
    Valve(byte pin, char name) : Output(pin) {
      this->name = name;
    }
  
    void on(){
      d_set(0);
      Serial.print(name);
      Serial.println(" valve ON");
    }
  
    void off(){
      d_set(1);
      Serial.print(name);
      Serial.println(" valve OFF");
    }
};

class SerialScreen {
  public:
    SerialScreen() {
    //Initializing Serial Monitor
      Serial.begin(115200);
      Serial.print("\n\n\n\n\n\n\n\n\nIC Assemblies Caribe");
      Serial.print("\nMercedes Acoustic Tube Assembler");
      Serial.print("\nIniciando Equipo....");
    }

    void show(byte state, int pressure) {
        if(state==0){
          Serial.print("\n\nPresion: ");
          Serial.println(pressure);
          Serial.print(" psi");
          Serial.print("\nModo Manual");
          Serial.print("\nUse comandos seriales");
        } else if(state==1){
          Serial.print("\n\nPresion: ");
          Serial.println(pressure);
          Serial.print(" psi");
          Serial.print("\nInserte Luer y Elbow y presione botones");
        } else if(state==2) {
          Serial.print("\n\nPresion: ");
          Serial.println(pressure);
          Serial.print(" psi");
          Serial.print("\nInserte tubo y presione botones");
        } else if (state==3) {
          Serial.print("\n\nPresion: ");
          Serial.println(pressure);
          Serial.print(" psi");
          Serial.print("\nTubo expandiendose");
          Serial.print("\nEspere............");
        } else if(state==4) {
          Serial.print("\n\nPresion: ");
          Serial.println(pressure);
          Serial.print(" psi");
          Serial.print("\nRecoja la unidad.");
        }
    }

    void pressure_invalid(int pressure, byte set_pressure, byte pressure_tolerance) {
      Serial.print("\n\nPresion: ");
      Serial.println(pressure);
      Serial.print(" psi");
      Serial.print("\nPresion debe estar entre ");
      Serial.println(set_pressure-pressure_tolerance);
      Serial.print(" y ");
      Serial.println(set_pressure+pressure_tolerance);
      Serial.print(" psi.\nFavor ajustar");
    }
};

class LCDScreen {
  private:
    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
  public:
    LCDScreen() {
      //Initializing LCD screen
      lcd.begin();
      lcd.clear();
      lcd.setCursor(0,0); lcd.print("IC Assemblies Caribe");
      delay(1000);
      lcd.setCursor(0,1); lcd.print(" Mercedes Acoustic");
      lcd.setCursor(0,2); lcd.print("   Tube Assembler");
      delay(1000);
      lcd.setCursor(0,3); lcd.print("Iniciando Equipo....");
      delay(1000);
    }

    void show(int state, int pressure) {
      if(state==0){
        lcd.clear();
        lcd.setCursor(0,0); lcd.print("Presion: ");
        lcd.setCursor(9,0); lcd.print(pressure);
        lcd.setCursor(11,0); lcd.print("psi");
        lcd.setCursor(0,1); lcd.print("Modo Manual");
        lcd.setCursor(0,2); lcd.print("Use comandos");
        lcd.setCursor(0,3); lcd.print("seriales");
      } else if(state==1){
        lcd.clear();
        lcd.setCursor(0,0); lcd.print("Presion: ");
        lcd.setCursor(9,0); lcd.print(pressure);
        lcd.setCursor(11,0); lcd.print("psi");
        lcd.setCursor(0,2); lcd.print("Inserte Luer y Elbow");
        lcd.setCursor(0,3); lcd.print("y presione botones");
      } else if(state == 2){
        lcd.clear();
        lcd.setCursor(0,0); lcd.print("Presion: ");
        lcd.setCursor(9,0); lcd.print(pressure);
        lcd.setCursor(11,0); lcd.print("psi");
        lcd.setCursor(0,2); lcd.print("Inserte tubo");
        lcd.setCursor(0,3); lcd.print("y presione botones");
      } else if(state==3){
        lcd.clear();
        lcd.setCursor(0,0); lcd.print("Presion: ");
        lcd.setCursor(9,0); lcd.print(pressure);
        lcd.setCursor(11,0); lcd.print("psi");
        lcd.setCursor(0,2); lcd.print("Tubo expandiendose.");
        lcd.setCursor(0,3); lcd.print("Espere..............");
      } else if(state==4){
        lcd.clear();
        lcd.setCursor(0,0); lcd.print("Presion: ");
        lcd.setCursor(9,0); lcd.print(pressure);
        lcd.setCursor(11,0); lcd.print("psi");
        lcd.setCursor(0,2); lcd.print("Recoja la unidad.");
      }
    }

    void pressure_act(int pressure, int pressure_prev){
      if(pressure!=pressure_prev){
        if(pressure < 10){
          lcd.setCursor(9,0);
          lcd.print(pressure);
          lcd.print("psi       ");
        } else {
          lcd.setCursor(9,0);
          lcd.print(pressure);
          lcd.print("psi      ");
        }
      }
    }

    void pressure_invalid(int set_pressure, int pressure_tolerance) {
      lcd.setCursor(0,1); lcd.print(" Presion debe estar ");
      lcd.setCursor(0,2); lcd.print("entre ");
      lcd.setCursor(6,2); lcd.print(set_pressure-pressure_tolerance);
      lcd.setCursor(8,2); lcd.print(" y ");
      lcd.setCursor(11,2); lcd.print(set_pressure+pressure_tolerance);
      lcd.setCursor(13,2); lcd.print(" psi    ");
      lcd.setCursor(0,3); lcd.print("Favor ajustar       ");
    }
};

class Screen {
  public:
    SerialScreen serial_s = SerialScreen();
    LCDScreen lcd_s = LCDScreen();
    Screen() {}

    void show(byte state, int pressure) {
      serial_s.show(state, pressure);
      lcd_s.show(state, pressure);
    }

    void pressure_invalid(int pressure, byte set_pressure, byte pressure_tolerance) {
      serial_s.pressure_invalid(pressure, set_pressure, pressure_tolerance);
      lcd_s.pressure_invalid(set_pressure, pressure_tolerance);
    }
};

class FiniteState {
  private:
    byte state=1;
    int pressure_prev=0, pressure_curr=0;
    const byte set_pressure=52, pressure_tolerance=5;
    bool pressure_OK=0;
    PressureSensor pressure_sensor = PressureSensor(A15);
    CylinderSensor cylinder_sensor = CylinderSensor(6);
    Button button = Button(29);
    Valve valve_V = Valve(38, "V");
    Valve valve_H = Valve(39, "H");
    Screen screen = Screen();
    //Variables for serial communication
    String     inputString = "help\n"; // a string to hold incoming data
    bool    stringComplete = 1;     // whether the string is completet

  public:
    FiniteState() {}

    void next() {
      if(state >= 4){
        delay(3000);
        state = 1;
      } else {
        state++;
      }

      screen.show(state, pressure_curr);
    }

    void pressure_check() {
      pressure_prev = pressure_curr;
      pressure_curr = pressure_sensor.pressure();

      if(pressure_curr != pressure_prev) screen.lcd_s.pressure_act(pressure_curr, pressure_prev);

      if(abs(pressure_curr-set_pressure)>pressure_tolerance){
        if(pressure_OK){
          screen.pressure_invalid(pressure_curr, set_pressure, pressure_tolerance);
          pressure_OK = 0;
        }
      } else if(!pressure_OK){
        pressure_OK = 1;
      }
    }

    void AutoManual(){
      inputString.setCharAt(0,' ');
      state=inputString.toInt();
      Serial.print("Mode = ");
      Serial.println(state);
      screen.show(state, pressure_curr);
    }

    void serialEvent(){ // ***************************Serial in
      while (Serial.available()) {
        char inChar = (char)Serial.read();            // get the new byte:
        if (inChar > 0)     {inputString += inChar;}  // add it to the inputString:
        if (inChar == '\n') { stringComplete = true;} // if the incoming character is a newline, set a flag so the main loop can do something about it: 
      }
      
      if (stringComplete) { 
        if (inputString=="hon\n")       {valve_H.on(); inputString="";}
        if (inputString=="hoo\n")       {valve_H.off();inputString="";}
        if (inputString=="von\n")       {valve_V.on(); inputString="";}
        if (inputString=="voo\n")       {valve_V.off();inputString="";}
        if (inputString.charAt(0)=='m') {AutoManual(); inputString="";}
        if (inputString !="") {Serial.println("BAD COMMAND="+inputString);}// Serial.print("\n"); }// "\t" tab      
        inputString = ""; stringComplete = false; // clear the string:
      }
    }

    void state_1(){
      valve_H.off();
      valve_V.off();
      
      if(button.is_pressed()){
        next();
        valve_V.on(); delay(700);
        valve_H.on();
      }
    }

    void state_2(){
      if(button.is_pressed()){
        valve_H.off();
        next();
      }
    }

    void state_3(){
      if(cylinder_sensor.is_activated()){
        delay(100);
        next();
      }
    }

    void state_4(){
      valve_V.off();
      next();
    }

    void state_machine() {
      serialEvent();
      pressure_check();

      if(pressure_OK){
        if(state == 1) state_1();
        else if(state == 2) state_2();
        else if(state == 3) state_3();
        else if(state == 4) state_4();
      }
    }
}

acoustic_tube_assembler = FiniteState();

void setup() {
// put your setup code here, to run once:
}

void loop() {
  acoustic_tube_assembler.state_machine();
}