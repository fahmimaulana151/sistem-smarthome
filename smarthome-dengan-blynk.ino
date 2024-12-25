//Library
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <MQ2.h>
#include <DHT.h>
#include <ThingSpeak.h>

//sambung ke Blynk
#define BLYNK_TEMPLATE_ID "TMPL6iQL4dUu_"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "Ci7BUOQGNSBoe3CDu0hQhSBxNqojymYd"
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

//Menambahkan LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <LCD_I2C.h>
LCD_I2C lcd(0x27, 20, 4);

//Sambung ke ThingSpeak
String apiKey = "C3R0IHAUEIMXGAD5";
unsigned long myChannelNumber = 2635101; 
const char * myWriteAPIKey = "C3R0IHAUEIMXGAD5"; 
const char *server = "api.thingspeak.com";
//Mengkonfigurasi WiFi
const char* ssid = "Pakek Ajah";
const char* password = "1sampai8";
//menambah kode untuk Blynk
BlynkTimer timer;

//tambah relay 1 menyala apabia deteksi api

#define MQ2 A0
int sensorValue = 0;
int Nilaisensor = 0;
int pinSensor = D0; // Misalnya, sensor di pin D2
int pinLED = D3;   // Misalnya, LED di pin D3
int pinBUZ = D1;
DHT dht(D4, DHT11); 
String url;
WiFiClient client;


void setup() {
 Serial.begin(9600);
 pinMode(pinSensor, INPUT);
  pinMode(pinLED, OUTPUT);
pinMode(pinBUZ, OUTPUT);
 //Mengkoneksikan WiFi
 WiFi.hostname("NodeMCU");
 WiFi.begin(ssid, password);
 //Menguji Koneksi WiFi
 while(WiFi.status() != WL_CONNECTED)
 {
   //Serial.println("Gagal terhubung ke jaringan WiFi!");
 digitalWrite(pinLED, LOW);
 delay(500);
 }
 //Serial.println("Berhasil terhubung ke jaringan WiFi!");
 digitalWrite(pinLED,HIGH);
 delay(500);
  // Inisialisasi ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
 //Membaca nilai sensor api
 int api = digitalRead(pinSensor);
 if(api==1)
 {
 //Tidak ada apa notifikasi tidak terkirim
 Serial.println("Api Diluar Jangkauan");
digitalWrite(pinBUZ, LOW);
 delay(500);
 }
 else
 {
 Serial.println("Api Terdeteksi");
  digitalWrite(pinBUZ, HIGH);
  delay(3000);
kirim_wa("Bahaya!!! \n Telah Ditemukan Titik Api"); 
 }
 delay(1000);

 //Membaca nilai gas
 int value = analogRead(MQ2);
 Serial.println(value);
  value = map(value, 0, 1024, 0, 100);
   if (value <= 30) {
    //Tidak ada apa notifikasi tidak terkirim
     digitalWrite(pinBUZ, LOW);
 delay(500);
 Serial.println("Nilai Sensor Gas Masih Aman");
  } else if (value > 30) {
    //Kirim Notifikasi
   Serial.println("Nilai Gas Sensor Tidak Aman");
    digitalWrite(pinBUZ, HIGH);
    delay(3000);
kirim_wa("Bahaya!!! \n Nilai Gas Sensor Tidak Aman"); 
  }

  // Membaca nilai DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
   if (isnan(h) || isnan(t)) {
    Serial.println("Gagal Membaca dari DHT sensor!");
    return;
  }
  ThingSpeak.writeField(myChannelNumber, 1,t, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 2,h, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 3,value, myWriteAPIKey);

  // Membaca nilai dari sensor
  Nilaisensor = analogRead(A0);  // membaca dari pin A0
  sensorValue = digitalRead(D4);  // membaca dari pin D4

  // Mengirim data ke ThingSpeak
  ThingSpeak.setField(3, value); // Mengirimkan ke Field 3 pada ThingSpeak yaitu Gas
  ThingSpeak.setField(2, h); // Mengirimkan ke Field 2 pada ThingSpeak yaitu Kelembapan
  ThingSpeak.setField(1, t);// Mengirimkan ke Field 1 pada ThingSpeak yaitu Suhu
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

void kirim_wa(String pesan)
{
url = "http://api.callmebot.com/whatsapp.php?phone=6282319825591&text="+ urlencode(pesan) +"&apikey=3276038";
//Mengirimkan pesan
postData();
}

void postData()
{
 //variabel status pesan wa
 int httpCode;
 //variabel protokol server http bot
 HTTPClient http;
 //eksekusi link URL
 http.begin(client, url);
 httpCode = http.POST(url);
 //menguji nilai variabel
 if(httpCode==200)
 {
 Serial.println("Notifikasi Terkirim");
 }
 else 
 {
 Serial.println("Notifikasi Tidak Terkirim");
 }
 http.end(); 
 }
 
 String urlencode(String str)
{
 String encodedString="";
 char c;
 char code0, code1, code2;
 for(int i=0; i<str.length(); i++)
 {
 c = str.charAt(i);
 if(c == ' '){
 encodedString += '+' ;
 }
 else if(isalnum(c))
 {
 encodedString += c;
 }
else
 {
 code1 = (c & 0xf) + '0';
 if((c & 0xf) > 9) {
 code1 = (c & 0xf) - 10 + 'A';
 }
 c = (c>>4)&0xf;
 code0 = c+'0';
 if(c > 9) {
 code0 = c - 10 + 'A';
 }
 code2 = '\0';
 encodedString += '%';
 encodedString += code0;
 encodedString += code1;
 }
 yield();
 }
 Serial.println(encodedString);
 return encodedString;
}


