#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress ip(192, 168, 0, 177);
const int numBytes = 256;

unsigned int localPort = 50200;      // local port to listen on
unsigned int remotePort_1 = 0;
IPAddress remoteIp_1;    

unsigned int remotePort_2 = 0;
IPAddress remoteIp_2;  

unsigned int remotePort_c = 0;
IPAddress remoteIp_c; 

int identificadorInicial_1=0x0; // es la letra inicial del paquete
int identificadorInicial_2=0x0; // es la letra inicial del paquete
int identificadorInicial_3=0x0; // es la letra inicial del paquete
int identificadorInicial_Maquina=0x0; // es la letra inicial del paquete

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

unsigned char buffer_ether[numBytes]={0};
int buffer_convertidor[numBytes]={0};
int buffer_serial[numBytes]={0};
unsigned char buffer_serial_salida[numBytes]={0};

int buffer_serial_3[numBytes]={0};
unsigned char buffer_serial_salida_3[numBytes]={0};

int buffer_maquina[numBytes]={0};
int buffer_respuesta[numBytes]={0};
 
const int pinLed1 = 13;
static byte ndx = 0;
static byte ndx_serial = 0; // tamaño del buffer serial 1
static byte ndx_serial_3 = 0; // tamaño del buffer serial 3
static byte ndx_maquina = 0; //// tamaño del buffer entrada de la maquina 
static byte ndx_respuesta = 0;//// tamaño del buffer respuesta de la maquina 

unsigned long StartTime=0;

unsigned long CurrentTime=0;
unsigned long ElapsedTime=0;

unsigned long timeChirping5s_ini=millis();
unsigned long timeChirping5s_fin=5000; // 5 segundos 

unsigned long timeChirping200_ini=millis();
unsigned long timeChirping200_fin=200; // 200 mili segundos 

unsigned long timeChirping10m_ini=millis();
unsigned long timeChirping10m_fin=600000; // 10min 

bool enviarChirp=false;
bool timer10min=false;

const int bitInicial=1;

struct{

long long id;
long long contadores[10];
int crc;

}cont;

void setup()
{
  Serial.begin(19200);
  Serial1.begin (19200, SERIAL_8N1, true);  // 9 bit mode
  Serial2.begin (19200, SERIAL_8N1, true);  // 9 bit mode
  Serial3.begin (19200, SERIAL_8N1, true);  // 9 bit mode
 

  if(Ethernet.begin(mac))
  {
    Udp.begin(localPort);
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
  }
  else
  {
    Serial.print("fallo DHCP");
    IPAddress ip(192, 168, 0, 177);
    Ethernet.begin(mac,ip);
    Udp.begin(localPort);
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
    
    }
  pinMode(pinLed1, OUTPUT);
  digitalWrite(pinLed1, LOW);

  Udp.setTimeout(5);
  Serial1.setTimeout(5);
  Serial2.setTimeout(5);
  Serial3.setTimeout(5);
}
 
void loop()
{
  
    int packetSize = Udp.parsePacket();
    
    if(packetSize)
    {
        
        Udp.read(buffer_ether, numBytes);

        

        buffer_convertidor[0] = buffer_ether[0];
       buffer_convertidor[bitInicial] = (buffer_ether[bitInicial]<<8) | buffer_ether[bitInicial+1];
       

        for(int l=bitInicial+1; l<packetSize;l++)
        {
          buffer_convertidor[l] = buffer_ether[l+1];
          }

         if(buffer_convertidor[0]==0x73) // s
            {
                  remoteIp_1 = Udp.remoteIP();
                  remotePort_1=Udp.remotePort();
                  identificadorInicial_1=buffer_convertidor[0];
                  for(int ls=0; ls<packetSize;ls++)
                  {
                    Serial1.write9bit(buffer_convertidor[ls]);
                  } 
                  Serial1.flush();
                  StartTime = millis();

          }//fin if letra
          else if(buffer_convertidor[0]==0x79) // y
            {

                  remoteIp_2 = Udp.remoteIP();
                  remotePort_2=Udp.remotePort();
                  identificadorInicial_2=buffer_convertidor[0];
                  for(int ls=0; ls<packetSize;ls++)
                  {
                    Serial3.write9bit(buffer_convertidor[ls]);
                  } 
                  Serial3.flush();
                  StartTime = millis();

          }//fin if letra
          else
          {

              remoteIp_c = Udp.remoteIP();
              remotePort_c=Udp.remotePort();
              identificadorInicial_3=buffer_convertidor[0];
              interpreteEntrada(buffer_ether);
            }
          
          memset(buffer_ether, 0, sizeof(buffer_ether));
           memset(buffer_convertidor, 0, sizeof(buffer_convertidor));
           //ndx_maquina=0;
           // memset(buffer_maquina, 0, sizeof(buffer_maquina));

        }

        //fin if client available
        if(Serial1.available())
        {

          while(Serial1.available() >0){
            int cl = Serial1.read();

            buffer_serial[ndx_serial]=cl;
            ndx_serial++;
                  if (ndx_serial >= numBytes) {
                      ndx_serial = numBytes - 1;
                  }

                         if(ndx_serial==bitInicial+1){
                             CurrentTime = millis();
                             ElapsedTime = CurrentTime - StartTime;
            
                            if(ElapsedTime > 127)
                            {
                                ElapsedTime = 127;
                            }
                          
                          ElapsedTime = ElapsedTime <<1;
                          buffer_serial_salida[0] = buffer_serial[0];
                          buffer_serial_salida[bitInicial] = (buffer_serial[bitInicial]>>8) & 0x00FF;
                          buffer_serial_salida[bitInicial + 1] = (buffer_serial[bitInicial]) & 0x00FF;
                        }
            }
          }
         else  if(Serial3.available())
        {

          while(Serial3.available() >0){
            int cl = Serial3.read();

            buffer_serial_3[ndx_serial_3]=cl;
            ndx_serial_3++;
                  if (ndx_serial_3 >= numBytes) {
                      ndx_serial_3 = numBytes - 1;
                  }

                         if(ndx_serial_3==bitInicial+1){
                             CurrentTime = millis();
                             ElapsedTime = CurrentTime - StartTime;
            
                            if(ElapsedTime > 127)
                            {
                                ElapsedTime = 127;
                            }
                          
                          ElapsedTime = ElapsedTime <<1;
                          buffer_serial_salida_3[0] = buffer_serial_3[0];
                          buffer_serial_salida_3[bitInicial] = (buffer_serial_3[bitInicial]>>8) & 0x00FF;
                          buffer_serial_salida_3[bitInicial + 1] = (buffer_serial_3[bitInicial]) & 0x00FF;
                        }
            }
          }
          else
          {
             
            if(ndx_serial>0){
             
            buffer_serial_salida[bitInicial] = buffer_serial_salida[bitInicial] | ElapsedTime;

             for(int sal=bitInicial+1;sal<ndx_serial;sal++)
              {
                    buffer_serial_salida[sal] = buffer_serial[sal-1];             
              }


                Udp.beginPacket(remoteIp_1, remotePort_1);
                Udp.write(buffer_serial_salida,ndx_serial);
                Udp.endPacket();

                
            //Serial.println(ndx_serial);
            ndx_serial=0;
            memset(buffer_serial, 0, sizeof(buffer_serial));
            memset(buffer_serial_salida, 0, sizeof(buffer_serial_salida));
            CurrentTime=0;
            StartTime=0;

            }
            else if(ndx_serial_3>0)
            {


               buffer_serial_salida_3[bitInicial] = buffer_serial_salida_3[bitInicial] | ElapsedTime;

             for(int sal=bitInicial+1;sal<ndx_serial_3;sal++)
              {
                    buffer_serial_salida_3[sal] = buffer_serial_3[sal-1];             
              }

              
               
                Udp.beginPacket(remoteIp_2, remotePort_2);
                Udp.write(buffer_serial_salida_3,ndx_serial_3);
                Udp.endPacket();

                
            //Serial.println(ndx_serial);
            ndx_serial_3=0;
            memset(buffer_serial_3, 0, sizeof(buffer_serial_3));
            memset(buffer_serial_salida_3, 0, sizeof(buffer_serial_salida_3));
            CurrentTime=0;
            StartTime=0;  
              
              }

          }


          //////Respuestas de la Maquina

          respuestaMaquina();
          //////////////////////////////


}

void respuestaMaquina()
{
        if((millis() - timeChirping5s_ini > timeChirping5s_fin) && timer10min==false && enviarChirp == false)
        {
     
            enviarChirp = true;
        
        }

        if((millis() - timeChirping200_ini > timeChirping200_fin) && enviarChirp == true && timer10min==false && identificadorInicial_Maquina!=0x0)
        {
     
            timeChirping200_ini = millis();

              buffer_respuesta[0]=identificadorInicial_Maquina;
              buffer_respuesta[1]=0x01;
              ndx_respuesta=3;
            
             
              for(int resp=0;resp<ndx_respuesta;resp++)
              {
              
                Serial2.write9bit(buffer_respuesta[resp]);
              
              }
              Serial2.flush();
              ndx_respuesta=0;
              memset(buffer_respuesta, 0, sizeof(buffer_respuesta));
        }

        if(millis() - timeChirping10m_ini > timeChirping10m_fin && enviarChirp == true && timer10min==false)
        {
     
            enviarChirp = false;
            timer10min=true;
        
        }


       

        if(Serial2.available())
        {

          timeChirping5s_ini = millis();
          timeChirping10m_ini = millis();
          enviarChirp = false;
          timer10min=false;
          int cm = Serial2.read();


         buffer_maquina[ndx_maquina]=cm;
         ndx_maquina++;
                  
                if (ndx_maquina >= numBytes) {
                    ndx_maquina = numBytes - 1;
                }
          
          }
          else
          {
            //Serial.println(buffer_maquina[0],HEX);
                         
              if(buffer_maquina[1]==0x180)
              {
               Serial.println("Exito");
               buffer_respuesta[0]=buffer_maquina[0]; 
               ndx_respuesta=1;
              }
  
              if(buffer_maquina[1]==0x181)
              {
               Serial.println("Exito general");
               buffer_respuesta[0]=buffer_maquina[0];
               buffer_respuesta[1]=0x00; // Evento
               ndx_respuesta=3;
              }
             
              if(buffer_maquina[1]==0x101 && buffer_maquina[2]==0x1C)
              {
               Serial.println("Exito Final");
               buffer_respuesta[0]=buffer_maquina[0];
               buffer_respuesta[1]=0x01;
               buffer_respuesta[2]=0x1C;
               buffer_respuesta[3]=0x01;
               buffer_respuesta[4]=0x02;
               buffer_respuesta[5]=0x03;
               buffer_respuesta[6]=0x04;
               buffer_respuesta[7]=0x05;
               buffer_respuesta[8]=0x06;
               buffer_respuesta[9]=0x07;
               buffer_respuesta[10]=0x08;
               buffer_respuesta[11]=0xCC;
               buffer_respuesta[12]=0xCC;
               ndx_respuesta=13;
              }
  
              if(buffer_maquina[1]==0x101 && buffer_maquina[2]==0x1F)
              {
               Serial.println("Exito Gaming ID");
               buffer_respuesta[0]=buffer_maquina[0];
               buffer_respuesta[1]=0x01;
               buffer_respuesta[2]=0x1F;
               
               buffer_respuesta[3]='S';
               buffer_respuesta[4]='I';
               buffer_respuesta[5]='1';
               buffer_respuesta[6]='2';
               buffer_respuesta[7]='3';
               buffer_respuesta[8]=0x01;
               buffer_respuesta[9]=20;
               buffer_respuesta[10]=0;
               buffer_respuesta[11]=0x02;
               buffer_respuesta[12]=0x03;
  
                buffer_respuesta[13]='R';
                buffer_respuesta[14]='u';
                buffer_respuesta[15]='l';
                buffer_respuesta[16]='0';
                buffer_respuesta[17]='3';
                buffer_respuesta[18]='7';
                buffer_respuesta[19]='9';
                buffer_respuesta[20]='7';
                buffer_respuesta[21]='3';
                buffer_respuesta[22]='0';
               
               buffer_respuesta[23]=0xCC;
               buffer_respuesta[24]=0xCC;
               ndx_respuesta=25;
              }

            

            if(ndx_respuesta > 0){
             
              for(int resp=0;resp<ndx_respuesta;resp++)
              {

                Serial2.write9bit(buffer_respuesta[resp]);
              
                }
                Serial2.flush();
                ndx_maquina=0;
                identificadorInicial_Maquina = buffer_maquina[0];
                memset(buffer_maquina, 0, sizeof(buffer_maquina));
          }

            ndx_respuesta=0;
            memset(buffer_respuesta, 0, sizeof(buffer_respuesta));
            
            }

  
            

  
  }

  void interpreteEntrada(unsigned char * bufferInterprete)
  {

    Serial.print("probando");
    
    }
