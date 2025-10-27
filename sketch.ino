/*
======== Sensor Inframerah + Gyroscope dengan 5 Actuator Servo ========
Nama : Tsaqif Jalaluddin Ahmad
NIM  : 24/537665/TK/59611
=======================================================================
*/


#include <ESP32Servo.h>       // untuk deklarasi objek servo dan mengoperasikan servo
#include <Adafruit_MPU6050.h> // untuk mengoperasikan sensor MPU6050, merekam data gyroscope
#include <Adafruit_Sensor.h>  // untuk sensor infra-merah, mensimulasikan pergerakan yang tertangkap sensor
#include <Wire.h>             // untuk menyiapkan pin MPU

// deklarasi sensor MPU6050 serta pin yang digunakan
Adafruit_MPU6050 mysensor;
#define pinSDA 14
#define pinSCL 12

// deklarasi semua servo
#define pinServo1 13
#define pinServo2 15 // pin servo gerakan melawan gyro-x, letaknya di bawah mikon

#define pinServo3 16
#define pinServo4 2  // pin servo gerakan searah gyro-y, letaknya di kanan mikon

#define pinServo5 26 // pin servo gerakan searah gyro-z, letaknya di atas mikon

Servo Servo_1; // deklarasi semua objek servo
Servo Servo_2;
Servo Servo_3;
Servo Servo_4;

Servo Servo_5;               // deklarasi servo 5, di atas mikon
const int default_cond = 90; // beberapa variabel yang dibutuhkan untuk
bool z_back = false;         // mengatur karakteristik pergerakan servo 5
unsigned long waktu_gerakanAkhir = millis(); 
// waktu sejak gerakan terakhir jika gyro sumbu z menjadi netral (0), 
// 'unsigned' agar nilainya tidak negatif (tidak ada waktu negatif)

#define pirPin 27 // pin milik sensor inframerah
int motion = 0;   // kondisi untuk menyatakan adakah pergerakan yang terdeteksi

void setup() {
  Serial.begin(115200);

  Wire.begin(pinSDA, pinSCL); //inisialisasi pin milik MPU

  Servo_1.attach(pinServo1);  // inisialisasi servo
  Servo_2.attach(pinServo2);
  Servo_3.attach(pinServo3);
  Servo_4.attach(pinServo4);
  Servo_5.attach(pinServo5);

  while (!mysensor.begin()) { // cek apakah MPU6050 siap merekam data
    Serial.println("MPU6050 not connected!");
    delay(1000);
  }

  Serial.println("MPU6050 ready!"); // notifikasi jika sensor MPU siap
  pinMode(pirPin, INPUT); // inisialisasi pin PIR sebagai input mikon (memberi masukan data motion)
}

sensors_event_t acceleration, gyros, temp; // alamat atau pointer variabel data MPU

void loop() {
  mysensor.getEvent(&acceleration, &gyros, &temp); // ambil nilai variabel yang terdapat di alamat masing-masing
  motion = digitalRead(pirPin); // baca input yang diberikan oleh PIR

  if(motion == LOW){
    float gyroX = gyros.gyro.x * 57.296; // konversi satuan radian/s menjadi degree/s
    float gyroY = gyros.gyro.y * 57.296; // karena .write objek servo menerima dalam satuan degree 0-180
    float gyroZ = gyros.gyro.z * 57.296;

    float sudut_for12 = constrain(90 - gyroX * 0.4, 0, 180); // mapping perhitungan konversi sudut agar
    float sudut_for34 = constrain(90 + gyroY * 0.4, 0, 180); // berada di rentang [0,180]
    float sudut_for5  = constrain(90 + gyroZ * 0.4, 0, 180);

    Servo_1.write(sudut_for12); // atur posisi setiap servo dengan sudut masing-masingnya
    Servo_2.write(sudut_for12);

    Servo_3.write(sudut_for34);
    Servo_4.write(sudut_for34);

      /* cek apakah perubahan pada gyroscope sumbu z lebih dari suatu batas. Nilai
      4 dapat dirubah, semakin kecil maka servo 5 makin sensitif dalam merespon perubahan
      kecepatan sudut */
      if(abs(gyroZ) > 4){
        Servo_5.write(sudut_for5);      // atur posisi servo searah dengan arah perubahan sudut
        waktu_gerakanAkhir = millis();  // rekam waktu pergerakan terakhir yang terjadi
        z_back = false;                 // tetapkan servo 5 belum waktunya kembali ke rest position (90 derajat)
      }
      else{

        if(!z_back){ // true jika sudah waktunya kembali ke kondisi awal

          if((millis() - waktu_gerakanAkhir) >= 1000){
            Servo_5.write(default_cond);
            z_back = true; 
            // kembali ke rest jika selisih waktu sistem (millis) dengan waktu gerakan terakhir sudah 1 detik atau lebih
          }
        
        }
      
      }
  
  }else{
    // respon unik semua servo jika terdeteksi gerakan oleh sensor PIR, ke kanan dan kiri semua servonya kecuali servo 5
    Servo_1.write(0);
    Servo_2.write(180);
    
    Servo_3.write(0);
    Servo_4.write(180);

    Servo_5.write(90);

  }
  delay(700);
}
