
//////////
/////////  All Serial Handling Code,
/////////  It's Changeable with the 'outputType' variable
/////////  It's declared at start of code.
/////////

void serialOutput(){   // Decide How To Output Serial.
  switch(outputType){
    case PROCESSING_VISUALIZER:
      sendDataToSerial('S', Signal);     // goes to sendDataToSerial function
      break;
    case SERIAL_PLOTTER:  // open the Arduino Serial Plotter to visualize these data
      Serial.print(BPM);
      Serial.print(",");
      Serial.print(IBI);
      Serial.print(",");
      Serial.println(Signal);
      break;
      /*
    case SAVE_AS_FILE:
      Serial.print("Initializing SD card...");
      if (!SD.begin(4)) //检测读取SD卡
      {
          Serial.println("initialization failed!");
          return;
      }
      Serial.println("initialization done.");
  DHT_Data = SD.open("data.txt", FILE_WRITE);//创建一个data.txt文件来存放DHT传感器的数据
       */
    default:
      break;
  }

}

//  Decides How To OutPut BPM and IBI Data
void serialOutputWhenBeatHappens(){
  switch(outputType){
    case PROCESSING_VISUALIZER:    
      sendDataToSerial('B',BPM);   // send heart rate with a 'B' prefix
      sendDataToSerial('Q',IBI);   // send time between beats with a 'Q' prefix
      break;

    default:
      break;
  }
}

//  Sends Data to Pulse Sensor Processing App, Native Mac App, or Third-party Serial Readers.
void sendDataToSerial(char symbol, int data ){
    Serial.print(symbol);
    Serial.println(data);
  }
