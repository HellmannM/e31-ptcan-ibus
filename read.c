
byte empfBytes[5];
bool newData = false;
String acc_pressed;
byte abbruch=0x21;
byte einaus=0x6F;
byte plus=0x2B;
byte minus=0x2D;


void setup() {

  Serial.begin(115200);

  Serial3.begin(9600);
  delay(200);
  if (Serial3.available() > 0) {
    Serial.print("ACC check ok");
  }
}

void loop() {
  if (Serial3.available() > 4)
  {
    for (int i = 0; i <= 3; i++) {
      empfBytes[i] = Serial3.read();
    }
    acc_pressed=getButton(empfBytes);

  }
}





byte getButton(byte *empfBytes){


    if (empfBytes[0] == 0x0c && empfBytes[1] == 0x1c && empfBytes[2] == 0xfc) {
      Serial.print("+");
       return plus;
       
    }
    if (empfBytes[0] == 0x70 && empfBytes[1] == 0x0c && empfBytes[2] == 0x1c) {
      Serial.print("+");
      return plus;
    }
    if (empfBytes [0] == 0x0c && empfBytes[2] == 0x1c && empfBytes[3] == 0x1c) {
      Serial.print("-");
      return minus;
    }
        if (empfBytes[1] == 0x1c && empfBytes[2] == 0x70 && empfBytes[3] == 0x1c) {
      Serial.print("A");
      return abbruch;
    }

    if (empfBytes[1] == 0x1c && empfBytes[2] == 0x70 &&  empfBytes[3] == 0xfc) {
      Serial.print("o");
      return einaus;
    }
    if (empfBytes[1] == 0xfc && empfBytes[2] == 0x1c &&  empfBytes[3] == 0x0c) {
      Serial.print("o");
       return einaus;
    }
    if (empfBytes[1] == 0x1c && empfBytes[2] == 0x70 &&  empfBytes[3] == 0xf0) {
      Serial.print("o");
       return einaus;
    }
  }