#include<SoftwareSerial.h>
#include <TinyGPS++.h>
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6u5T4m8Eq"
#define BLYNK_TEMPLATE_NAME "giamsat"
#define BLYNK_AUTH_TOKEN "w_o-WfvUEkzblaoVP1ok6QnfbAkPPUCn"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "w_o-WfvUEkzblaoVP1ok6QnfbAkPPUCn";
char ssid[] = "Raptor";
char pass[] = "hoilamgi";

char data,chay,l;
char    dl[16]="";

float gtnd,gtph,luuluong;
long  gtduc,gttds,kc;

long   gt,ttled,quet;

float   latf,lngf;
long    latn,lngn;
unsigned long     cht,dvt,chn,dvn;
unsigned long     chtt,dvtt,chnt,dvnt;
unsigned long lastSend = 0;

static const int TXPin = 12, RXPin = 14;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

WidgetLED led(V9);
WidgetTerminal terminal(V10);

BLYNK_CONNECTED() 
{
  Blynk.syncAll();
}

BLYNK_WRITE(V8)
{
  gt = param.asInt();
  if(gt==1)     Serial.print(1);
  else          Serial.print(2);
}

void setup()
{
  Serial.begin(9600);
  pinMode(16,OUTPUT);
  ss.begin(9600);
  digitalWrite(16,0);       delay(500);
  digitalWrite(16,1);      delay(500);
  digitalWrite(16,0);       delay(500);
  digitalWrite(16,1);      delay(500);
  digitalWrite(16,0);       delay(500);
  digitalWrite(16,1);      delay(500);
  Blynk.begin(auth, ssid, pass);
  digitalWrite(16,0);
  chay=0;
  ttled=0;
  quet=0;
  led.off();      
}
void loop()
{
  Blynk.run(); 
  while(ss.available()>0)
  if(gps.encode(ss.read()))
  displayInfo();
  if((gtduc>500)||(gtph>11000)||(gtnd>4000)||(kc>60)||(gttds>100))
  {
    if(ttled==0)
    {
      ttled=1;
      led.on();
    }
  }
  else
  {
    if(ttled==1)
    {
      ttled=0;
      led.off();
    }
  }
  if(Serial.available() > 0)
  {
    data = Serial.read(); 
    if(data=='.')
    {
      if(dl[0]=='a')
      {
        gtph=(dl[1]-0x30)*1000+(dl[2]-0x30)*100+(dl[3]-0x30)*10+(dl[4]-0x30);
        kc=(dl[5]-0x30)*1000+(dl[6]-0x30)*100+(dl[7]-0x30)*10+(dl[8]-0x30);
        Blynk.virtualWrite(V0, gtph/100); 
        Blynk.virtualWrite(V1, kc); 
        // Serial.print("GT ph:"); 
        // Serial.println(gtph/100);  
      }
      else if(dl[0]=='b')
      {
        gtnd=(dl[1]-0x30)*1000+(dl[2]-0x30)*100+(dl[3]-0x30)*10+(dl[4]-0x30);
        gtduc=(dl[5]-0x30)*1000+(dl[6]-0x30)*100+(dl[7]-0x30)*10+(dl[8]-0x30);
        Blynk.virtualWrite(V2, gtnd); 
        Blynk.virtualWrite(V3, gtduc); 
        // Serial.print("GT nd:"); 
        // Serial.println(gtnd);  
        // Serial.print("GT duc:"); 
        // Serial.println(gtduc);  
      }
      else if(dl[0]=='c')
      {
        gttds=(dl[1]-0x30)*1000+(dl[2]-0x30)*100+(dl[3]-0x30)*10+(dl[4]-0x30);
        luuluong=(dl[5]-0x30)*1000+(dl[6]-0x30)*100+(dl[7]-0x30)*10+(dl[8]-0x30);
        Blynk.virtualWrite(V4, gttds); 
        Blynk.virtualWrite(V5, luuluong/100); 
        // Serial.print("luuluong:"); 
        // Serial.println(luuluong);  
      }
      // else if(dl[0]=='c')
      // {
      //   tt=dl[1]-0x30;
      //   if(tt==1)     led.on();
      //   else          led.off();
      // }
      for(l=0;l<16;l++)
      {
        dl[l]=0x20;
      }
      chay=0;
    }
    else
    {
       dl[chay]=data;
       chay++;
    }
  }

  quet++;
  delay(1);
  if (millis() - lastSend >= 30000) {
    lastSend = millis();

    // Vẫn giữ nguyên V6, V7 để gửi lat/lng
    Blynk.virtualWrite(V6, latf / 1000000.0);
    Blynk.virtualWrite(V7, lngf / 1000000.0);

    // Tạo link Google Maps và gửi lên V10
    String link = "https://maps.google.com/?q=" + String(latf / 1000000.0, 6) + "," + String(lngf / 1000000.0, 6);
    terminal.clear();              // Xóa dòng trước
    terminal.println(link);        // Gửi link kèm xuống dòng
    terminal.flush();              // Gửi ngay
  }
}

void displayInfo()
{
  if(gps.location.isValid())
  {
    latf=gps.location.lat()*1000000;        // vi do
    lngf=gps.location.lng()*1000000;        // kinh di
    
  }
}

