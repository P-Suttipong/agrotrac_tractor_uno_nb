#include "AIS_NB_BC95.h"
#include "HMC5983.h"
#include "EasyScheduler.h"
#include <Wire.h>

HMC5983 compass;

Schedular Task1;

int DUE_Address = 17711;
int message = 1;
int c = 0; //initial compass
String apnName = "devkit.nb";
String serverIP = "203.150.107.176"; // Your Server IP
String serverPort = "17711"; // Your Server Port

String udpData = "HelloWorld";
String nb_status = "disconnected";

String NewNBData = "";
String NBData = "";

String messages[3];
boolean isStart = false;

AIS_NB_BC95 AISnb;

const long interval = 5000;  //millisecond
unsigned long previousMillis = 0;

long cnt = 0;


void setup()
{
  AISnb.debug = true;

  Serial.begin(9600);

  AISnb.setupDevice(serverPort);

  String ip1 = AISnb.getDeviceIP();
  delay(1000);

  pingRESP pingR = AISnb.pingIP(serverIP);
  previousMillis = millis();

  //I2C
  Wire.begin(DUE_Address);
  while (nb_status != "connected") {
    NB_IOT("tryToConnected", 5000);
  }
  NewNBData = AISnb.datas;
  NBData = NewNBData;

  Serial.println("Initialize Compass");
  compass.begin();

  Task1.start();

}
void loop()
{
  Task1.check(Send_compass, 100);

  NB_IOT("recieve", 5000);
  NewNBData = AISnb.datas;
  if (NBData != NewNBData) {
    char charBuf[100];
    NewNBData.toCharArray(charBuf, 100);
    splint_string(charBuf);
    Serial.print("Order: ");
    Serial.println(messages[0]);
    int value1 = messages[1].toInt();
    int val1_high = value1 / 256;
    int val1_low = value1 % 256;
    Serial.print("1 High: ");
    Serial.println(val1_high);
    Serial.print("1 Low: ");
    Serial.println(val1_low);
    int value2 = messages[2].toInt();
    int val2_high = value2 / 256;
    int val2_low = value2 % 256;
    Serial.print("2 High: ");
    Serial.println(val1_high);
    Serial.print("2 Low: ");
    Serial.println(val1_low);
    if (messages[0] == "forward") {
      Send_I2C(1);
      Send_I2C(val1_high);
      Send_I2C(val1_low);
    }
    else if (messages[0] == "backward")
    {
      Send_I2C(2);
      Send_I2C(val1_high);
      Send_I2C(val1_low);
    }
    else if (messages[0] == "left")
    {
      Send_I2C(3);
      Send_I2C(val1_high);
      Send_I2C(val1_low);
    }
    else if (messages[0] == "right")
    {
      Send_I2C(4);
      Send_I2C(val1_high);
      Send_I2C(val1_low);
    }
    else if (messages[0] == "area")
    {
      Send_I2C(5);
      Send_I2C(val1_high);
      Send_I2C(val1_low);
      Send_I2C(val2_high);
      Send_I2C(val2_low);
    }
    else if (messages[0] == "setting")
    {
      Send_I2C(6);
      Send_I2C(val1_high);
      Send_I2C(val1_low);
      Send_I2C(val2_high);
      Send_I2C(val2_low);
    }
    else if (messages[0] == "start")
    {
      Send_I2C(7);
    }
    else if (messages[0] == "up")
    {
      Send_I2C(8);
    }
    else if (messages[0] == "down") {
      Send_I2C(9);
    }
    Serial.print("SEND I2C : ");
    Serial.println(messages[0]);
  }
  NBData = NewNBData;
}

void NB_IOT(String data, int timer) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= timer)
  {
    cnt++;

    // Send data in String
    UDPSend udp = AISnb.sendUDPmsgStr(serverIP, serverPort, data);

    //Send data in HexString
    //udpDataHEX = AISnb.str2HexStr(udpData);
    //UDPSend udp = AISnb.sendUDPmsg(serverIP, serverPort, udpDataHEX);
    previousMillis = currentMillis;
    Serial.println(AISnb.datas);
  }
  AISnb.waitResponse();
  if (AISnb.datas == "connected") {
    nb_status = "connected";
  }
}

void Send_compass() {
  if (messages[0] == "start" || messages[0] == "left") {
    c = compass.read();
    if (c == -999) {
      Serial.println("Reading error, discared");
    } else {
      Send_I2C(c);
    }
  }
}

void Send_I2C(int data) {
  //Serial.println("ส่งค่าให้ Arduino ตัวที่ 2");
  Wire.beginTransmission( DUE_Address );
  Wire.write(data);
  delay(300);
  Wire.endTransmission();
}

void splint_string(char sz[]) {
  char *p = sz;
  char *str;
  int counter = 0;
  while ((str = strtok_r(p, "-", &p)) != NULL) {
    counter++;
    //    Serial.print(counter + String(". "));
    //    Serial.println(str);
    messages[counter - 1] = str;
  }
  counter = 0;
}
