import controlP5.*;
import processing.serial.*;

ControlP5 controlP5;
DropdownList ports;
Serial port;
int value;
String[] comList ;
boolean serialSet;
boolean Comselected = false;

void setup() {
  size(500,500);
  controlP5 = new ControlP5(this);
  comList = Serial.list();
  ports = controlP5.addDropdownList("ports")
    .setPosition(100, 100)
    .setSize(200, 100)
    .setBarHeight(20)
    .setItemHeight(20)
    .setCaptionLabel("Select COM port")
    .addItems(comList);
}

//callback function for dropdownlist item selection
void controlEvent(ControlEvent theEvent) {
  value = int(theEvent.getValue());
  Comselected = true;
}

void startSerial(String[] portsList){
  port = new Serial(this, portsList[value], 9600);
  serialSet = true;
}


void draw() {
  while(Comselected == true && serialSet == false)
  {
    startSerial(comList);
  }
  //set the background color of the sketch
  background(0);
  
  //Do some stuff, for example, since the serial connection is set up (once the user has selected a port), sending/receive serial data ;).
}