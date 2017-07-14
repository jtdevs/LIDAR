import processing.serial.*;
Serial lidar;
byte[] inBuffer = new byte[2];
int[] distance_array = new int[1601];//have the worst case in mind, that is 1600 measurements
int distance;
int i = 0;
float x,y;
int steps = 100;
float degperstep = 180.0/steps;
boolean readyflag = false;
boolean constant_scan = false;
boolean busy = false;
void setup(){
  //lidar = new Serial(this, "/dev/tty.usbserial-AH03L4JG");//parity=none databits=8 stopbits=1 baudrate = 9600 are defaults
  lidar = new Serial(this, "COM4");
  lidar.buffer(2);
  size(600,300);
  screen();
}

void draw(){
  if(readyflag){//If and only if all the data has arrived, we will update the screen
    screen();
    fill(#00FA00);
    stroke(#00FA00);
    for(int k = 0; k < (steps+1); k++){
      x = cos(radians(k*degperstep))*(distance_array[k]+4);
      y = sin(radians(k*degperstep))*(distance_array[k]+4);
      ellipse(300+x,300-y,1,1);
      println(distance_array[k]+4, k*degperstep);
    }
    readyflag = false;
  }
  if(busy == false && constant_scan == true){
    lidar.write(0x04);
    busy = true;
  }
}

void keyTyped(){
  if(key == '0' && busy == false){//100 steps
    lidar.write(0x01);
    steps = 100;
    degperstep = 180.0/steps;
  }
  else if(key == '1' && busy == false){//200 steps
    lidar.write(0x02);
    steps = 200;
    degperstep = 180.0/steps;
  }
  else if(key == '2' && busy == false){//400 steps
    lidar.write(0x03);
    steps = 400;
    degperstep = 180.0/steps;
  }
  else if(key == '3' && busy == false){//continuous scan
    lidar.write(0x04);
  }
  else if(key == '4' && busy == false){//scan
    lidar.write(0x05);
  }
  else if(key == '5' && busy == false){//scan the other guy
    lidar.write(0x55);
  }
}

void serialEvent(Serial lidar){
  lidar.readBytes(inBuffer);
  lidar.clear();
  distance = (int(inBuffer[0]) << 8) + int(inBuffer[1]);//MSBF
  distance_array[i] = distance;
  println(distance, i);
  i++;
  if(i>steps){
    i=0;
    readyflag = true;
    println("ready!");
    busy = false;
  }
}

void screen(){
  int j;
  background(0);
  for (j = 0; j <= 6; j++) {
    strokeWeight(1);
    stroke(0, 55, 0);
    line(300, 300, 300 + cos(radians(30*j))*300, 300 - sin(radians(30*j))*300);
  }
  noFill();
  strokeWeight(1);
  for (j = 1; j <= 6; j++){
    stroke(0, 255-(30*j), 0);
    ellipse(300, 300, (100*j), (100*j));
  }
  fill(255);
  stroke(255);
  ellipse(300, 300, 4,4);
}