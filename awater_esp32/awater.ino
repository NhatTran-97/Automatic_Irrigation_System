
#include <WiFiClient.h>
#include <HTTPClient.h>
const char *ssid = "AndroidAP";
const char *pass = "123456789";
long last = 0;
int nhietdo, doam, van1,van2, van3, van4, van5, van6, van7,van8 = 0;


bool data_complete = false;uint8_t countbyte=0;uint8_t array_receive[40]; uint8_t data_array[40];
String SendESP = ""; // send data to server

uint8_t dieu_kien ; uint8_t dk; // conditional to send uart send uart
String Gio_on_str = "";String Phut_on_str = ""; String Thu2_str = ""; String Thu3_str = ""; String Thu4_str = "";String Thu5_str = ""; String Thu6_str =""; String Thu7_str = "";
String Thu8_str = "";String Gio_str = "";String Phut_str = "";String Giay_str = "";String Thu_str = "";String Ngay_str = "";String Thang_str = "";String Nam_str ="";

String auto1_str = ""; String auto2_str = "";String van1_str = "";String van2_str = "";String van3_str = "";String van4_str = "";String van5_str = "";String van6_str = "";
String van7_str = "";String van8_str = "";String dk_van1 = ""; String dk_van2 = ""; String dk_van3 =""; String dk_van4 = "";String dk_van5 = ""; String dk_van6 = "";String dk_van7 = ""; String dk_van8 = "";
String Config_val_str = ""; 

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  last = millis();
}
void loop() 
{
 if (WiFi.status() == WL_CONNECTED)
  {
    if (millis() - last > 2000)
    {
      //send_dulieu();
      Read_DieuKhien_PHP();
      chuongtrinhcambien();
      Send_Data_PHP( String(nhietdo) , String(doam) , String(van1) , String(van2) , String(van3), String(van4), String(van5), String(van6), String(van7), String(van8));
      last = millis();
    }
  }

 send_data_STM(); 
 read_uart();
 
}

void read_uart(void)
{
  while (Serial2.available()> 0)
{
  int inChar = Serial2.read();
  
  data_array[countbyte] = inChar;
  countbyte++;
  if (inChar == 120)
  {
    data_complete = true;
  }
  if (data_complete)
  {
    // vanstate 1-8 <=> byte 27 -> byte 33
    Serial.print("Data nhan tu STM = ");
    Serial.println(data_array[27]);
    Serial.println(data_array[28]);
    Serial.println(data_array[29]);
    Serial.println(data_array[30]);
    Serial.println(data_array[31]);
    Serial.println(data_array[32]);
    Serial.println(data_array[33]);
    Serial.println(data_array[34]);
    countbyte = 0;
    data_complete = false;
    }

}
  
  }

  void chuongtrinhcambien()
{
  nhietdo = 15;
  doam = 75;

  van1 = data_array[27];
  van2 = data_array[28];
  van3 = data_array[29];
  van4 = data_array[30];
  van5 = data_array[31];
  van6 = data_array[32];
  van7 = data_array[33];
  van8 = data_array[34];
 Serial.print("van 1: "); Serial.println(van1);
  
}

void Send_Data_PHP(String nhietdo , String doam , String van1 , String van2 , String van3, String van4, String van5, String van6, String van7,String van8)
{
  HTTPClient http;
  //http.begin("http://ip:port//folder chứa file InsertDB.php ( htdocs) /InsertDB.php");

  //http://10.40.252.252:8000/DatabasePHP/InsertDB.php
  http.begin("http://192.168.43.34:8000/Awater/InsertDB.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "nhietdo="   + String(nhietdo)  // $_POST['nhietdo']; trùng với "nhietdo=" 
                           + "&doam=" +  String(doam) + "&van1="  + String(van1)+ "&van2="  + String(van2)+  "&van3="  + String(van3)+  "&van4="  + String(van4)
  
                           +  "&van5="  + String(van5)
                           +  "&van6="  + String(van6)
                           +  "&van7="  + String(van7)
                           +  "&van8="  + String(van8)
                           + "";
  // hiển thị dữ liệu request

  Serial.print(".Data Send PHP:");
  Serial.println(httpRequestData);
  // Send HTTP POST request về databasephp
  int httpResponseCode = http.POST(httpRequestData); // phương thức post là gửi dữ liệu đi 

  if (httpResponseCode > 0)
  {

    //digitalWrite(LED2, LOW);
    //delay(200);
    //digitalWrite(LED2, HIGH);
  }
  else
  {
    Serial.print("Error: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  //=================================================================================
}

void Read_DieuKhien_PHP()
{
 // http://10.40.252.252:8000/DatabasePHP/Getdatadieukhien.php
  HTTPClient http;
  const char* host = "http://192.168.43.34:8000/Awater/Getdatadieukhien.php";
  http.begin(host);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.GET();// phương thức get là lấy dữ liệu về 

  // nếu lấy thành công httpCode = 200
  if (httpCode == 200)
  {
 
    String payload = http.getString();
  //  Serial.print(bien);
    Serial.print(".Data Read PHP:");
    
     SendESP =  payload ;
     Serial.println(SendESP);
   

   Gio_on_str = splitString(payload,",",0);Phut_on_str = splitString(payload,",",1); Thu2_str = splitString(payload,",",2);  Thu3_str = splitString(payload,",",3);  
   Thu4_str = splitString(payload,",",4);Thu5_str = splitString(payload,",",5);  Thu6_str = splitString(payload,",",6);  Thu7_str = splitString(payload,",",7);Thu8_str = splitString(payload,",",8);
   Gio_str = splitString(payload,",",19); Phut_str = splitString(payload,",",20); Giay_str = splitString(payload,",",25);
   Thu_str = splitString(payload,",",24); Ngay_str = splitString(payload,",",21); Thang_str = splitString(payload,",",22); Nam_str = splitString(payload,",",23);auto1_str = splitString(payload,",",9);  auto2_str = splitString(payload,",",10);
     
  van1_str = splitString(payload,",",11); van2_str = splitString(payload,",",12);van3_str = splitString(payload,",",13); van4_str = splitString(payload,",",14);van5_str = splitString(payload,",",15); van6_str = splitString(payload,",",16);
  van7_str = splitString(payload,",",17); van8_str = splitString(payload,",",18); dk_van1 = splitString(payload,",",26);  dk_van2 = splitString(payload,",",27); 
  dk_van3 = splitString(payload,",",28);  dk_van4 = splitString(payload,",",29);dk_van5 = splitString(payload,",",30);  dk_van6 = splitString(payload,",",31);
  dk_van7 = splitString(payload,",",32);  dk_van8 = splitString(payload,",",32);Config_val_str = splitString(payload,",",34);  String control_str = splitString(payload,",",35);

     dieu_kien = control_str.toInt();
     Serial.println(dieu_kien);
    
    
    /* uint8_t gio_on = Gio_on_str.toInt();uint8_t phut_on = Phut_on_str.toInt();
      uint8_t thu2 = Thu2_str.toInt();uint8_t thu3 = Thu3_str.toInt();uint8_t thu4 = Thu4_str.toInt();uint8_t thu5 = Thu5_str.toInt();
      uint8_t thu6 = Thu6_str.toInt(); uint8_t thu7 = Thu7_str.toInt(); uint8_t thu8 = Thu8_str.toInt();
      uint8_t gio = Gio_str.toInt();uint8_t phut = Phut_str.toInt(); uint8_t giay = Giay_str.toInt();
      
      uint8_t thu = Thu_str.toInt();uint8_t ngay = Ngay_str.toInt(); uint8_t thang = Thang_str.toInt(); uint16_t nam = Nam_str.toInt();
      uint8_t auto1 = auto1_str.toInt();uint8_t auto2 = auto2_str.toInt();
      uint8_t phut_V1 =  van1_str.toInt();  uint8_t phut_V2 = van2_str.toInt(); uint8_t phut_V3 =  van3_str.toInt();  uint8_t phut_V4 = van4_str.toInt();
     uint8_t phut_V5 =  van5_str.toInt();  uint8_t phut_V6 = van6_str.toInt(); uint8_t phut_V7 =  van7_str.toInt();  uint8_t phut_V8 = van8_str.toInt();
    uint8_t vanstate1 = dk_van1.toInt(); uint8_t vanstate2 = dk_van2.toInt();  uint8_t vanstate3 = dk_van3.toInt(); uint8_t vanstate4 = dk_van4.toInt();
    uint8_t vanstate5 = dk_van5.toInt(); uint8_t vanstate6 = dk_van6.toInt(); uint8_t vanstate7 = dk_van7.toInt(); uint8_t vanstate8 = dk_van8.toInt();
    uint8_t config_val = Config_val_str.toInt();*/
    
/*
  if( dk != dieu_kien)
  {
    dk = dieu_kien;



    Serial2.write('`');
    array_receive[0] = Gio_str.toInt(); array_receive[1] = Phut_str.toInt();array_receive[2] = 0;array_receive[3] = Thu_str.toInt();
    array_receive[4] =  Ngay_str.toInt(); array_receive[5] = Thang_str.toInt();array_receive[6] =  22;array_receive[7] = Gio_on_str.toInt();
    array_receive[8] = Phut_on_str.toInt(); array_receive[9] = Thu8_str.toInt();array_receive[10] = Thu2_str.toInt();array_receive[11] = Thu3_str.toInt();
    array_receive[12] = Thu4_str.toInt(); array_receive[13] = Thu5_str.toInt();array_receive[14] = Thu6_str.toInt();array_receive[15] = Thu7_str.toInt();
     array_receive[16] = auto1_str.toInt(); array_receive[17] = auto2_str.toInt();
     
    array_receive[18] = van1_str.toInt(); array_receive[19] = van2_str.toInt();array_receive[20] = van3_str.toInt();array_receive[21] = van4_str.toInt();
    array_receive[22] = van5_str.toInt(); array_receive[23] = van6_str.toInt();array_receive[24] = van7_str.toInt();array_receive[25] = van8_str.toInt();
    
    array_receive[26] = dk_van1.toInt(); array_receive[27] = dk_van2.toInt();array_receive[28] = dk_van3.toInt();array_receive[29] = dk_van4.toInt();
    array_receive[30] = dk_van5.toInt(); array_receive[31] = dk_van6.toInt();array_receive[32] = dk_van7.toInt();array_receive[33] = dk_van8.toInt();
    array_receive[34] = 0; array_receive[35] =  Config_val_str.toInt();



    for(int k = 0; k<36; k++)
    {
      Serial2.write(array_receive[k]);
    }
    Serial2.write('x');
    
    }
  */
    

      /*
   Serial2.write('`');
      Serial2.write(gio); Serial2.write(phut); Serial2.write(giay);
      Serial2.write(thu); Serial2.write(ngay); Serial2.write(thang);Serial2.write(22); Serial2.write(gio_on); Serial2.write(phut_on);
      Serial2.write(thu2); Serial2.write(thu3); Serial2.write(thu4);Serial2.write(thu5); Serial2.write(thu6); Serial2.write(thu7);Serial2.write(thu8);
      Serial2.write(auto1);Serial2.write(auto2);
      Serial2.write(phut_V1);Serial2.write(phut_V2);Serial2.write(phut_V3);Serial2.write(phut_V4);Serial2.write(phut_V5);Serial2.write(phut_V6);
      Serial2.write(phut_V7);Serial2.write(phut_V8); Serial2.write(vanstate1); Serial2.write(vanstate2); Serial2.write(vanstate3); Serial2.write(vanstate4); 
      Serial2.write(vanstate5);Serial2.write(vanstate6);  Serial2.write(vanstate7); Serial2.write(vanstate8);Serial2.write(0); Serial2.write(config_val); 
   Serial2.write('x');  */ 
  }
  http.end();
}


void send_data_STM(void)
{
  if(dk != dieu_kien)
 {
  dk = dieu_kien;
  Serial2.write('`');
    array_receive[0] = Gio_str.toInt(); array_receive[1] = Phut_str.toInt();array_receive[2] = 0;array_receive[3] = Thu_str.toInt();
    array_receive[4] =  Ngay_str.toInt(); array_receive[5] = Thang_str.toInt();array_receive[6] =  22;array_receive[7] = Gio_on_str.toInt();
    array_receive[8] = Phut_on_str.toInt(); array_receive[9] = Thu8_str.toInt();array_receive[10] = Thu2_str.toInt();array_receive[11] = Thu3_str.toInt();
    array_receive[12] = Thu4_str.toInt(); array_receive[13] = Thu5_str.toInt();array_receive[14] = Thu6_str.toInt();array_receive[15] = Thu7_str.toInt();
     array_receive[16] = auto1_str.toInt(); array_receive[17] = auto2_str.toInt();
     
    array_receive[18] = van1_str.toInt(); array_receive[19] = van2_str.toInt();array_receive[20] = van3_str.toInt();array_receive[21] = van4_str.toInt();
    array_receive[22] = van5_str.toInt(); array_receive[23] = van6_str.toInt();array_receive[24] = van7_str.toInt();array_receive[25] = van8_str.toInt();
    
    array_receive[26] = dk_van1.toInt(); array_receive[27] = dk_van2.toInt();array_receive[28] = dk_van3.toInt();array_receive[29] = dk_van4.toInt();
    array_receive[30] = dk_van5.toInt(); array_receive[31] = dk_van6.toInt();array_receive[32] = dk_van7.toInt();array_receive[33] = dk_van8.toInt();
    array_receive[34] = 0; array_receive[35] =  Config_val_str.toInt();


    
    for(int k = 0; k<36; k++)
    {
      Serial2.write(array_receive[k]);
      Serial.println(array_receive[k]);
    }
      Serial2.write('x');
  }
  }


String splitString(String str, String delim, uint16_t pos)
{
  String tmp = str;

  for(int i=0; i<pos; i++){
    tmp = tmp.substring(tmp.indexOf(delim)+1);

    if(tmp.indexOf(delim)== -1 
    && i != pos -1 )
      return "";
  }
  return tmp.substring(0,tmp.indexOf(delim));
}
