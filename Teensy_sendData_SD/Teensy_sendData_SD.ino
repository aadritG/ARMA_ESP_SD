
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>

File myFile;
int pinCS = 6;

// Json object to store data
StaticJsonDocument<2000> json;
// This string use to store JSON data
String publish_data;
bool go = 1;

#define DATA_SIZE 4800

uint16_t myarray[DATA_SIZE];

void ClearBuffer(){
  while(Serial3.available()){
    Serial3.read();
  }
}

void setup() {

  pinMode(pinCS, OUTPUT); 

  if (SD.begin(10))
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }  

  // put your setup code here, to run once:
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);

  Serial.begin(115200);
  Serial.println("setup complete");
  Serial3.begin(115200);
  



/* ----------------programming mode GREEN LED---------------------- */
//
//  digitalWrite(23, LOW);
//  digitalWrite(22, LOW);
//  digitalWrite(21, HIGH);
//  
//  pinMode(5, OUTPUT);
//  pinMode(14, OUTPUT);
//  delay(100);
//  digitalWrite(5, HIGH);
//  digitalWrite(14, HIGH);
//  delay(100);
//
//  digitalWrite(14, LOW);  //off 
//  delay(1000);
//  digitalWrite(5, LOW); //low gpio0
//  delay(1000);
//  digitalWrite(14, HIGH); //on


//  delay(2000);
//  digitalWrite(5, HIGH);  //hi gpio0



/* ----------------normal mode reset RED LED---------------------- */
//
  digitalWrite(23, HIGH);
  digitalWrite(22, LOW);
  digitalWrite(21, LOW);
  
  pinMode(14, OUTPUT);
  pinMode(5, OUTPUT);
  delay(100);
  digitalWrite(14, HIGH);
  digitalWrite(5, HIGH);
  delay(100);
  digitalWrite(14, LOW);  //off 
  delay(1000);
  digitalWrite(14, HIGH); //on

  
//  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  digitalWrite(23, HIGH);
  digitalWrite(22, LOW);
  digitalWrite(21, LOW);
  delay(1000);
  digitalWrite(23, LOW);
  digitalWrite(22, HIGH);
  digitalWrite(21, LOW);
  Serial.println("blinkyboy");

  //Create/Open File
  SD.remove("test.csv");  //remove file if it exists
  myFile = SD.open("test.csv", O_READ | O_WRITE | O_CREAT);   
  if (myFile) 
  {
    Serial.println("test.csv opened");
  }
  // if the file didn't open, print an error:
  else 
  {
    Serial.println("error opening test.txt");
  }  

  while(Serial3.available())
  {
    String readstr = Serial3.readString();
    Serial.println( readstr );
  }

  String lineindex;  
  uint8_t ack;

 delay(10000);

  if (go)
  {
    myFile.print("PATIENT ID XXX: ");
    
    for (uint16_t i = 0; i < DATA_SIZE; i++)
    {
      myarray[i] = i;
      char* my_ptr = (char*)&myarray[i];

      myFile.print(myarray[i]);
      myFile.print(", ");
      
      while (Serial3.availableForWrite() < 5)
        {
            Serial.println( "S3 buffer full, waiting" );
        }
        
      ClearBuffer();
      Serial3.write(*my_ptr);
      
      //wait for ack for byte 1
      while(!Serial3.available()) {Serial.println("Wait for ACK ");}
      ack = Serial3.read();
      while(ack != 35)
      {
        Serial.print("wrong ACK byte1 ");
        Serial.println(ack);
        ack = Serial3.read();
      }
      
      Serial3.write(*(my_ptr+1));
      Serial.println(myarray[0],HEX);
      //wait for ack for byte 2
      while(!Serial3.available()) {Serial.println("Wait for ACK ");}
      ack = Serial3.read();
      while(ack != 64)
      {
        if(Serial3.available())
        {
          Serial.print("wrong ACK byte2 ");
          Serial.print(ack);
          Serial.print(" for element ");
          Serial.println(i);        
          ack = Serial3.read();
        }
      }
    }
    go = 0;

    myFile.print("\n");
    myFile.close(); // close the file
    Serial.println("Done writing");
  }  

  while(Serial3.available())
  {
    String readstr = Serial3.readString();
    Serial.println( readstr );
  }

  // Reading the file
  myFile = SD.open("test.csv");
  if (myFile) {
    Serial.println("Read:");
    // Reading the whole file
    while (myFile.available()) {
      Serial.write(myFile.read());
   }
    myFile.close();
  }
  else {
    Serial.println("error opening test.txt");
  }
  

  delay(10000);

}
