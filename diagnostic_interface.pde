import processing.serial.*;

Serial arduino;    //Serial object for arduino
String inString;   //Input from arduino serial
int[] servos;      //Angles for servo motors
PFont courier;     //Text Font

void settings() {
  size(1200, 800);
}

void setup() {
  /**
  Initializes the serial communication with Arduino Uno and builds window for GUI
  */
  arduino = new Serial(this, Serial.list()[0], 9600);  //constructs Serial
  background(51);
  String[] fontlist = PFont.list();
  printArray(fontlist);
  courier = createFont("Courier", 32);
  
  
}

void draw() {
  /**
  Draw method for GUI. Builds a console, a robot visualization, runtime indacator,
  servo angles, and a live feed of the commands being sent.
  */
  background(51);
  //Create Robotic Model space
  fill(0,0,0);
  stroke(232,232,232);
  rect(20, 75, 500, 500);
  //Create Servo Angle Monitors
  //Servo A (Left leg, front, top joint)
  rect(1200-170, 40, 150, 100);
  //Servo B (Left leg, front, middle joint)
  rect(1200-2*170, 40, 150, 100);
  //Servo C (Left leg, front, lower joint)
  rect(1200-3*170, 40, 150, 100);
  //Servo D (Left leg, rear, top joint)
  rect(1200-2*170, 2*40+100, 150, 100);
  //Servo E (Left leg, rear, middle joint)
  rect(1200-3*170, 2*40+100, 150, 100);
  //Servo F (Left leg, rear, lower joint)
  rect(1200-170, 2*40+100, 150, 100);
  //Servo G (Right leg, front, top joint)
  rect(1200-170, 3*40+2*100, 150, 100);
  //Servo H (Right leg, front, middle joint)
  rect(1200-2*170, 3*40+2*100, 150, 100);
  //Servo J (Right leg, front, lower joint)
  rect(1200-3*170, 3*40+2*100, 150, 100);
  //Servo K (Right leg, rear, top joint)
  rect(1200-170, 4*40+3*100, 150, 100);
  //Servo L (Right leg, rear, middle joint)
  rect(1200-2*170, 4*40+3*100, 150, 100);
  //Servo M (Right leg, rear, lower joint)
  rect(1200-3*170, 4*40+3*100, 150, 100);
  //Create Console output for Arduino serial
  fill(255,255,255);
  stroke(232,232,232);
  rect(0, 600, 1200, 200);
  
  //For information from the serial input, a run loop is necessary
  while(arduino.available() > 0) {
    servos = serialEvent(arduino);
    
  }
}

int[] serialEvent(Serial port) {
  /**
  Takes string data from the serial communication and convertes it into an integer
  array. This is then returned for use.
  */
  int [] data;
  inString = port.readStringUntil('\n');          //Reads println data from port
  String[] dataStr = splitTokens(inString, ",");  //splits data into array
  //Needs 12 data points– one for each servo– sets each string type to an integer
  if (dataStr.length == 12) {
    data = new int[12];
    for(int i = 0; i <= 12; i++) {
      data[i] = int(dataStr[i]);
    }
  }
  //For exception handling, returns zeros array if there isn't enough data
  else {
    data = new int[12];
    for(int i = 0; i<=2 ; i++) {
      data[i] = 0;
    }
  }
  return data;
} //END