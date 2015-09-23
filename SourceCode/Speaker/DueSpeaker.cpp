// Do not remove the include below
#include "Arduino.h"

#include "logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include <string.h>

#include <SPI.h>

#include <Ethernet.h>
#include <Dns.h>
#include <EthernetClient.h>
#include <util.h>
#include <EthernetUdp.h>
#include <Dhcp.h>
#include <EthernetServer.h>


#define MAX_SONG 6

const char *getResult(OCStackResult result);

PROGMEM const char TAG[] = "ArduinoServer";

int gLightUnderObservation = 0;
void createLightResource();
uint32_t  millis_prv = millis();
uint32_t  save_time;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

/* Structure to represent a Light resource */
typedef struct SPEAKER{
    OCResourceHandle handle;
    int64_t state;
    int64_t volume;
    int64_t time;
    int64_t present_song;
    int64_t next_song;
    
   
} Speaker;

static Speaker speaker;

// Arduino Ethernet Shield
int ConnectToNetwork()
{
	
	  if(Ethernet.begin(mac)== 0){
	  
	  
	}
  delay(1000);
    

  IPAddress  ip = Ethernet.localIP();
  
    return 0;
}

void makeString(char a, int64_t in, char* st){
	
	st[0] = a;
	
	 if(in >= 100){
		st[1] = '3' ;
		st[2] = in/100 + '0';
		st[3] = (in/10)%10 + '0';
		st[4] = in%10 + '0';
		st[5] = '\0';
  }else if(in >= 10){
		st[1] = '2';
		st[2] = in/10 + '0';
		st[3] = in%10 + '0';
		st[4] = '\0'; 
  }else{
		st[1] = '1';
		st[2] = in + '0';
		st[3] = '\0'; 
  }
   
	
	
}






OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest,
                                        void *callbackParam)
{
    OCEntityHandlerResult ehRet = OC_EH_OK;
    OCEntityHandlerResponse response = {0};
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        //OC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return OC_EH_ERROR;
    }

    if(entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        //OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));

        if(OC_REST_GET == entityHandlerRequest->method)
        {
            OCRepPayloadSetUri(payload, "/iotar/speaker");
            OCRepPayloadSetPropInt(payload, "state", speaker.state);
            OCRepPayloadSetPropInt(payload, "volume",speaker.volume);
            OCRepPayloadSetPropInt(payload, "time", speaker.time);
            OCRepPayloadSetPropInt(payload, "present_song",speaker.present_song);
            OCRepPayloadSetPropInt(payload, "next_song", speaker.next_song);
            
            
        }
        else if(OC_REST_PUT == entityHandlerRequest->method)
        {
			
			
			
			OCRepPayload* pay = (OCRepPayload*) entityHandlerRequest-> payload;
			
			int64_t order = 0;
			int64_t temp = 0;
			char in[10];
			OCRepPayloadGetPropInt(pay, "order", &order);
			
			if(order == 30){   //재생 
				OCRepPayloadGetPropInt(pay, "temp", &temp);
				
				makeString('a', temp, (char*)&in); 
				
				speaker.present_song = temp;
				Serial1.println(in);
				millis_prv = millis();
				save_time = 0;
				speaker.state = 1;
			}else if(order == 31){  //볼륨 설정 
				
				OCRepPayloadGetPropInt(pay, "temp", &temp);
				
				makeString('v', temp, (char*)&in); 
				
				speaker.volume = temp;
				Serial1.println(in);
				
			}else if(order == 32){  //다음 노래 설정 
				
				OCRepPayloadGetPropInt(pay, "temp", &temp);
				
				makeString('d', temp, (char*)&in); 
				
				speaker.next_song = temp;
				Serial1.println(in);
				
			}else if(order == 33){  // 현재 아무일 안하고 있으면 0, 재생중 1, 일시정지 2. 로 세팅.
				
				Serial1.println('x');
				
			}else if(order == 34) { //정지.
				
				Serial1.println('s');
				
			}else if(order == 35) { // +1 볼륨.
				speaker.volume++;
				if(speaker.volume >= 100){
					speaker.volume = 100;
				}
				makeString('v', speaker.volume, (char*)&in); 
				Serial1.println(in);
				
				
			}else if(order == 36) { // -1 볼륨.
				
				speaker.volume--;
				if(speaker.volume < 1){
					speaker.volume = 1;
				}
				makeString('v', speaker.volume, (char*)&in); 
				Serial1.println(in);
				
			}else if(order == 37){ // 일시정지, 일시정지 풀기.
				
				Serial1.println('p');
				if(speaker.state == 1){
					speaker.state = 2;
					save_time = speaker.time;
				}else{
					speaker.state = 1;
					millis_prv = millis();
				}
			}else if(order == 38){   //다음노래 재생 
				speaker.present_song++;
				if(speaker.present_song > MAX_SONG){
					speaker.present_song = 1;
				}
				
				makeString('a', speaker.present_song, (char*)&in); 
				
				Serial1.println(in);
				millis_prv = millis();
				save_time = 0;
				speaker.state = 1;
			}else if(order == 39){ // 이전 노래 재생 
				
				speaker.present_song--;
				if(speaker.present_song < 1){
						speaker.present_song = MAX_SONG;
				}
				
			
				
				makeString('a', speaker.present_song, (char*)&in); 
				Serial1.println(in);
				millis_prv = millis();
				save_time = 0;
				speaker.state = 1;
				
				
			}

			OCRepPayloadSetUri(payload, "/iotar/speaker");
			OCRepPayloadSetPropInt(payload, "time", speaker.time);
			OCRepPayloadSetPropInt(payload, "vol", speaker.volume);
			OCRepPayloadSetPropInt(payload, "track", speaker.present_song);
			OCRepPayloadSetPropInt(payload, "state", speaker.state);
          
            
            
            
        }

        if (ehRet == OC_EH_OK)
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehRet;
            response.payload = (OCPayload*) payload;
            response.numSendVendorSpecificHeaderOptions = 0;
            memset(response.sendVendorSpecificHeaderOptions, 0,
                    sizeof response.sendVendorSpecificHeaderOptions);
            memset(response.resourceUri, 0, sizeof response.resourceUri);
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Send the response
            if (OCDoResponse(&response) != OC_STACK_OK)
            {
             //   OC_LOG(ERROR, TAG, "Error sending response");
                ehRet = OC_EH_ERROR;
            }
        }
    }
    if (entityHandlerRequest && (flag & OC_OBSERVE_FLAG))
    {
        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
           // OC_LOG (INFO, TAG, PCF("Received OC_OBSERVE_REGISTER from client"));
            gLightUnderObservation = 1;
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
           // OC_LOG (INFO, TAG, PCF("Received OC_OBSERVE_DEREGISTER from client"));
            gLightUnderObservation = 0;
        }
    }
    OCRepPayloadDestroy(payload);
    return ehRet;
}



//The setup function is called once at startup of the sketch
void setup()
{
 
   
    Serial.begin(115200);
    Serial1.begin(115200);
  
 
	
   
     if (ConnectToNetwork() != 0)
    {
   
        return;
    }
	
  
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
  
        return;
    }
	
    createLightResource();
    
    speaker.state = 0;
    speaker.volume = 40;
    speaker.next_song = 1;
    speaker.present_song = 1;
    speaker.time = 0;
    
    
    
   
}





void input_order(char* in){
   
   uint8_t len;
   
   len = Serial.read() - '0';
   
   
   for(int i=0; i< len; i++){
     
     in[i] = Serial.read();
   }
   
   in[len] = '\0';
   
 }
 
 void input_order1(char* in){
   
   uint8_t len;
   
   len = Serial1.read() - '0';
   
   
   for(int i=0; i< len; i++){
     
     in[i] = Serial1.read();
   }
   
   in[len] = '\0';
   
 }
 
 

void loop()
{
	

   
    if (OCProcess() != OC_STACK_OK)
    {
     
        return;
    }
    
    
    byte in;
    char input[10];
    
     if(Serial.available()) { 
		in = Serial.read();
		
		if(in == 't'){   // ex) 23sec <- t223,   241sec <- t3241, 3sec <- t13
			
			
			input_order((char*)&input);
      
            speaker.time = atoi(input);
			
		}else if( in == 's'){    //ex) track023 <-  s223
			
			input_order((char*)&input);
      
            speaker.present_song = atoi(input);
			
		}else if( in == 'x'){   //mp3 state if) stop -> x10, play -> x11, pause -> x12;
			
			input_order((char*)&input);
      
            speaker.state = atoi(input);
			
		}
			
		
	}
	
	delay(200);
	
	if(speaker.state == 0){
		speaker.time = 0;
		
	}else if(speaker.state == 2){
		
	}else{
	 speaker.time = ((millis() -  millis_prv)/1000) + save_time;
	} 
	
    
}



void createLightResource()
{
    speaker.state = false;
    OCStackResult res = OCCreateResource(&speaker.handle,
            "core.speaker",
            OC_RSRVD_INTERFACE_DEFAULT,
            "/iotar/speaker",
            OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE|OC_OBSERVABLE);
 
}



