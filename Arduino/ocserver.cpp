// Do not remove the include below
#include "Arduino.h"

#include "logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include <string.h>


//#ifdef ARDUINOWIFI
// Arduino WiFi Shield
#include <SPI.h>
//#include <WiFi.h>
//#include <WiFiUdp.h>
//#else   
// Arduino Ethernet Shield
#include <Ethernet.h>
#include <Dns.h>
#include <EthernetClient.h>
#include <util.h>
#include <EthernetUdp.h>
#include <Dhcp.h>
#include <EthernetServer.h>
//#endif

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
	//OC_LOG(DEBUG, TAG, PCF("ConnectToNetwork is starting..."));
    // Note: ****Update the MAC address here with your shield's MAC address****

	  
	  if(Ethernet.begin(mac)== 0){
	  
	  
		//  OC_LOG(DEBUG, TAG, PCF("Failed to configure Ethernet using DHCP"));
   
  
  // give the Ethernet shield a second to initialize:
}
  delay(1000);
    

  IPAddress  ip = Ethernet.localIP();
  // OC_LOG_V(INFO, TAG, "IP Address:  %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return 0;
}
//#endif //ARDUINOWIFI

// This is the entity handler for the registered resource.
// This is invoked by OCStack whenever it recevies a request for this resource.

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
			
			
			//OC_LOG_V(DEBUG, TAG, "put \n");
			
			  //풋 올때 데이터 어떻게 받지?? 
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
					save_time = speaker.time+2;
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
    // Add your initialization code here
    // Note : This will initialize Serial port on Arduino at 115200 bauds
   // OC_LOG_INIT();
   
    Serial.begin(115200);
    Serial1.begin(115200);
  
  //  OC_LOG(DEBUG, TAG, PCF("OCServer is starting..."));
//    OC_LOG(DEBUG, TAG, PCF("OCServer is starting..."));
	
    // Connect to Ethernet or WiFi network
   
     if (ConnectToNetwork() != 0)
    {
     //   OC_LOG(ERROR, TAG, PCF("Unable to connect to network"));
        return;
    }
	
    // Initialize the OC Stack in Server mode
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
     //   OC_LOG(ERROR, TAG, PCF("OCStack init error"));
        return;
    }
	
    // Declare and create the example resource: Light
    createLightResource();
    
    speaker.state = 0;
    speaker.volume = 40;
    speaker.next_song = 1;
    speaker.present_song = 1;
    speaker.time = 0;
    
    
    
    //OC_LOG(DEBUG, TAG, PCF("소라 놋북 터진 날 15.07.31 ..."));
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
 
 
// The loop function is called in an endless loop
void loop()
{
	//OC_LOG(DEBUG, TAG, PCF("doing loop funcation..."));
    // This artificial delay is kept here to avoid endless spinning
    // of Arduino microcontroller. Modify it as per specific application needs.

    // Give CPU cycles to OCStack to perform send/recv and other OCStack stuff
    if (OCProcess() != OC_STACK_OK)
    {
      //  OC_LOG(ERROR, TAG, PCF("OCStack process error"));
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
	
	delay(1000);
	
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
   // OC_LOG_V(INFO, TAG, "Created Light resource with result: %s", getResult(res));
}

const char *getResult(OCStackResult result) {
    switch (result) {
    case OC_STACK_OK:
        return "OC_STACK_OK";
    case OC_STACK_INVALID_URI:
        return "OC_STACK_INVALID_URI";
    case OC_STACK_INVALID_QUERY:
        return "OC_STACK_INVALID_QUERY";
    case OC_STACK_INVALID_IP:
        return "OC_STACK_INVALID_IP";
    case OC_STACK_INVALID_PORT:
        return "OC_STACK_INVALID_PORT";
    case OC_STACK_INVALID_CALLBACK:
        return "OC_STACK_INVALID_CALLBACK";
    case OC_STACK_INVALID_METHOD:
        return "OC_STACK_INVALID_METHOD";
    case OC_STACK_NO_MEMORY:
        return "OC_STACK_NO_MEMORY";
    case OC_STACK_COMM_ERROR:
        return "OC_STACK_COMM_ERROR";
    case OC_STACK_INVALID_PARAM:
        return "OC_STACK_INVALID_PARAM";
    case OC_STACK_NOTIMPL:
        return "OC_STACK_NOTIMPL";
    case OC_STACK_NO_RESOURCE:
        return "OC_STACK_NO_RESOURCE";
    case OC_STACK_RESOURCE_ERROR:
        return "OC_STACK_RESOURCE_ERROR";
    case OC_STACK_SLOW_RESOURCE:
        return "OC_STACK_SLOW_RESOURCE";
    case OC_STACK_NO_OBSERVERS:
        return "OC_STACK_NO_OBSERVERS";
    case OC_STACK_ERROR:
        return "OC_STACK_ERROR";
    default:
        return "UNKNOWN";
    }
}

