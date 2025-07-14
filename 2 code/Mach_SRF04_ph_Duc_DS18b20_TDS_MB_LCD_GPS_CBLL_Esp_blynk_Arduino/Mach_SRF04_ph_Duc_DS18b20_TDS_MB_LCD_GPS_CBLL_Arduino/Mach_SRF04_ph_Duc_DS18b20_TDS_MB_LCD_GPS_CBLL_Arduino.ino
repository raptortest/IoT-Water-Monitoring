#include <Wire.h>
#include <SoftwareSerial.h>
SoftwareSerial sim(8,7);
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimerOne.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

#define ONE_WIRE_BUS 12          // Cài đặt chân GPIO sử dụng
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int   duc=A2;
int   tds=A1;
int   ph=A0;

const int t1 = 4;     // chân trig của HC-SR04
const int e1 = 5;     // chân echo của HC-SR04

int   mb=9;

long  gtph,gtduc,ducluu, gttds,gtnd;  
long  gtpht,gtduct, gttdst,gtndt, tdsluu;

float ndf,phf;
long  chay,quet,nd,phluu,ndluu;
long  chph,dvph,chnd,dvnd;

long  adc,i,kc1,kct;
char  data;
int   tt,ttt,ttmb;

unsigned long int avgValue;
float b;
int buf[10], temp;

long    luuluong,luuluongt,xung,encoder;
float   ll,llt,x;
String textMessage;
char luu[100];
long  chayg,ttg;
int   kich;

// unsigned long     cht,dvt,chn,dvn;
// unsigned long     d1,d2,d3,d4,d5,d6;

void setup() 
{
  Serial.begin(9600);
  sim.begin(115200);
  sim.println("ATE1");// Bật chế độ echo để nhận phản hồi từ module
  delay(100);  // Đợi một chút để module thực thi lệnh
  pinMode(t1,OUTPUT);
  pinMode(mb,OUTPUT);
  pinMode(e1,INPUT);
  lcd.init();
  lcd.backlight();
  sensors.begin(); // Bắt đầu đọc cảm biến   
  pinMode(duc,INPUT);
  pinMode(ph,INPUT);
  digitalWrite(mb,0);

  pinMode(2,INPUT_PULLUP);      // chan ngat 
  attachInterrupt(0, demxung, FALLING);
  Timer1.initialize(1000000); //Khởi động ngắt, 
  Timer1.attachInterrupt(dem_thoi_gian);


  lcd.setCursor(0,0);
  lcd.print("INIT SIM....");
  lcd.setCursor(0,1);
  lcd.print("PLEASE WAIT....");
  delay(15000);
  sim.println("AT+CMGF=1\r\n"); 
  delay(100);
  sim.println("AT+CNMI=2,2,0,0,0\r\n");
  delay(1000);
  sim.println("AT+CMGS=\"+84961092595\"\r\n");
  delay(1000);
  sim.println("START SUCCESSFUL");
  delay(1000);
  sim.println((char)26);
  delay(1000);
  doc_tds();
  doc_cam_bien_duc();
  sensors.requestTemperatures();
  nd = sensors.getTempCByIndex(0);
  gtnd = nd * 100;
  doc_ph();
  doc_kc1();  
  phluu = gtph;
  ducluu = gtduc;
  ndluu = gtnd;
  tdsluu = gttds;
  sendSMS();
  // ---------------------    



  lcd.init();
  lcd.backlight();
  chay=0;
  quet=0;
  ttt=10;
  tt=0;
  kich=1;
  ttmb=0;
  encoder=2382;
}

void loop() 
{
  hienthi();
  nhan_gt();
  chay++;
  delay(10);
  if(chay<10)
  {
    delay(20); 
  }
  else
  {
    chay=0;
    doc_kc1();
    doc_tds();
    sensors.requestTemperatures(); 
    nd=sensors.getTempCByIndex(0);
    gtnd = nd * 100;
    doc_ph();
    doc_cam_bien_duc();
    gui_tin_hieu();
    hienthi();
  }

  if(ttmb==1)       digitalWrite(mb,1);
  else              digitalWrite(mb,0);
  kiem_tra();
}

void demxung()
{
  xung++;  
}

void dem_thoi_gian()
{
  x=xung;
  ll=x*60/encoder;
  xung=0;
}

void doc_kc1()
{
  delayMicroseconds(50);
  digitalWrite(t1,0);   // tắt chân tri
  delayMicroseconds(20);
  digitalWrite(t1,1);   // phát xung từ chân tri
  delayMicroseconds(20);   // xung có độ dài 5 microSecond
  digitalWrite(t1,0);   // tắt chân tri
  kc1 = pulseIn(e1,HIGH);      // tra ve don vi uS
  kc1 = kc1/58;       // cm     // theo datasheet srf04
  if(kc1>200)      kc1=200;
  // Serial.print("KC1:");
  // Serial.println(kc1);
}

void doc_ph()
{
  for (int i = 0; i < 10; i++)
  {
    buf[i] = analogRead(ph);
    delay(10);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  avgValue = 0;
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];
  float pHVol = (float)avgValue * 5.0 / 1024 / 6;
  float phValue = -5.60 * pHVol + 21.19;
  gtph=phValue*100;
}

void doc_cam_bien_duc()
{
  adc=0;
  for(i=0;i<10;i++)
  {
    adc = adc + analogRead(duc);
    delay(1);
  }
  adc=adc/10;
  adc = adc*5000/1023;      // adc chay tu 0 - 5000
  gtduc=1000-(adc/5);         // ntu chay tu 0 - 1000
  if(gtduc>999)      gtduc=999;  
}

void doc_tds()
{
  adc=0;
  for(i=0;i<10;i++)
  {
    adc=adc+analogRead(tds);  
    delay(1);
  }
  adc=adc/10;
  adc=adc*5000/1023;
  if(adc>2300)     adc=2300; 
  gttds=adc/2.3;  // 0-2300  / 2.3
  if(gttds>999)     gttds=999;
}

void hienthi()
{
  phf=gtph;
  phf=phf/100;

  lcd.setCursor(1,0);
  lcd.print("pH:");
  lcd.print(phf);
  lcd.print(" ");
  lcd.print("TE:");
  lcd.print(nd);
  lcd.write(0xdf);
  lcd.print("C  ");

  lcd.setCursor(0,1);
  lcd.print("WF: ");
  lcd.print(ll);
  lcd.print("l/m  ");

  lcd.setCursor(0,2);
  lcd.print("TURB: ");
  lcd.print(gtduc);
  lcd.print("NTU   ");

  lcd.setCursor(0,3);
  lcd.print("DIS:");
  lcd.print(kc1);
  lcd.print("cm  ");
  lcd.print("TDS:");
  lcd.print(gttds);
  lcd.print("ppm  ");
}

void nhan_gt()
{
  if(Serial.available() > 0)
  {
    data = Serial.read();
    if(data=='1')         
    {
      ttmb=1;
    }        
    else if(data=='2')    
    {
      ttmb=0; 
    } 
  }
}


void gui_tin_hieu()
{
  if((gtpht!=gtph)||(kct!=kc1))     // khi nao thay doi thi gui 1 lan 
  {
    kct=kc1;
    gtpht=gtph;
    Serial.print("a");  
    Serial.write(gtph/1000%10+0x30);
    Serial.write(gtph/100%10+0x30);
    Serial.write(gtph/10%10+0x30);
    Serial.write(gtph%10+0x30);
    Serial.write(kc1/1000%10+0x30);
    Serial.write(kc1/100%10+0x30);
    Serial.write(kc1/10%10+0x30);
    Serial.write(kc1%10+0x30);
    Serial.print(".");        // ket thuc chuoi gui
    delay(100);
  }

  if((gtndt!=gtnd)||(gtduct!=gtduc))     // khi nao thay doi thi gui 1 lan 
  {
    gtndt=gtnd;
    gtduct=gtduc;
    Serial.print("b");  
    Serial.write(nd/1000%10+0x30);
    Serial.write(nd/100%10+0x30);
    Serial.write(nd/10%10+0x30);
    Serial.write(nd%10+0x30);
    Serial.write(gtduc/1000%10+0x30);
    Serial.write(gtduc/100%10+0x30);
    Serial.write(gtduc/10%10+0x30);
    Serial.write(gtduc%10+0x30);
    Serial.print(".");        // ket thuc chuoi gui
    delay(100);
  }

  llt=ll;
  llt=llt*100;
  luuluong=llt;
  if((gttdst!=gttds)||(luuluongt!=luuluong))     // khi nao thay doi thi gui 1 lan 
  {
    gttdst=gttds;
    luuluongt=luuluong;
    Serial.print("c");  
    Serial.write(gttds/1000%10+0x30);
    Serial.write(gttds/100%10+0x30);
    Serial.write(gttds/10%10+0x30);
    Serial.write(gttds%10+0x30);
    Serial.write(luuluong/1000%10+0x30);
    Serial.write(luuluong/100%10+0x30);
    Serial.write(luuluong/10%10+0x30);
    Serial.write(luuluong%10+0x30);
    Serial.print(".");        // ket thuc chuoi gui
    delay(100);
  }
}

void sendSMS()
{
  chph=phluu/100;
  dvph=phluu%100;
  chnd=ndluu/100;
  dvnd=ndluu%100;
  sprintf(luu, "pH: %ld.%ld - TURBIDITY: %ld - TEMPERATURE: %ld.%ld - TDS: %ld - DISTANCE: %ld", chph, dvph, ducluu, chnd, dvnd, tdsluu, kc1);
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"+84961092595\"\r\n");        //doi sdt can gui toi di   0347145673
  delay(1000);
  sim.println(luu);
  delay(1000);
  sim.println((char)26);
  delay(1000);
}

void xoa_tin()
{
  sim.print("AT+CMGF=1\r"); 
  delay(100);
  sim.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);  
}

void goi()
{
  sim.println("ATD+84961092595;");// thay thế số đt của bạn vào đây.
  delay(100);
}

void ket_thuc()
{
  sim.println("ATH");   // kết thúc cuộc gọi. 
}

void kiem_tra()
{
  if((gtduc>500)||(gtph>7000)||(gtnd>3800)||(kc1>60)||(gttds>100))
  {
    if(kich==1)
    {
      kich=0;
      phluu=gtph;
      ducluu=gtduc;
      ndluu=gtnd;
      tdsluu = gttds;
      sendSMS(); // Gửi tin nhắn cảnh báo NGAY
      tt=1;
    }
  }else {
    // Nếu các thông số trở lại bình thường => cho phép gửi lại nếu vượt ngưỡng lần sau
    kich = 1;
    tt = 0;
  }
}

