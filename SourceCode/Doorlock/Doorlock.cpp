// Do not remove the include below
#include "Arduino.h"

#include "logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include <string.h>

#include <SPI.h>
#include <Servo.h>
#include <Ethernet.h>
#include <Dns.h>
#include <EthernetClient.h>
#include <util.h>
#include <EthernetUdp.h>
#include <Dhcp.h>
#include <EthernetServer.h>


const char *getResult(OCStackResult result);

PROGMEM const char TAG[] = "ArduinoServer";

int gLightUnderObservation = 0;
void createLightResource();
uint32_t  millis_prv = millis();

int servoPin = 31;
Servo servo;
int angle = 0;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF};
typedef struct DOORLOCK{
    OCResourceHandle handle;
   
    
   
} Doorlcok;

static Doorlcok doorlock;
/* Structure to represent a Light resource */


// Arduino Ethernet Shield
int ConnectToNetwork()
{
	
	  if(Ethernet.begin(mac)== 0){
	  
	  
	}
  delay(1000);
    

  IPAddress  ip = Ethernet.localIP();
   OC_LOG_V(INFO, TAG, "IP Address:  %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return 0;
}


OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest,
                                        void *callbackParam)
{
    OCEntityHandlerResult ehRet = OC_EH_OK;
    OCEntityHandlerResponse response = {0};
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
       
        return OC_EH_ERROR;
    }

    if(entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
     

        if(OC_REST_GET == entityHandlerRequest->method)
        {
            OCRepPayloadSetUri(payload, "/iotar/doorlock");
          
        }
        else if(OC_REST_PUT == entityHandlerRequest->method)
        {
			
			
			OC_LOG_V(DEBUG, TAG, "put \n");
			
			  //풋 올때 데이터 어떻게 받지?? 
			OCRepPayload* pay = (OCRepPayload*) entityHandlerRequest-> payload;
			
			int64_t order = 0;
			int64_t temp = 0;
			char in[10];

				for(angle =90; angle>0; angle--){  // 열리기 
					servo.write(angle);
					delay(15); 
				}
			
				

				delay(5000);
   
				for(angle =0; angle<90; angle++){  //닫히기   
					servo.write(angle);
					delay(15); 
				}
				

        
			
			OCRepPayloadSetUri(payload, "/iotar/doorlock");
			
            
            
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
	
    OC_LOG(DEBUG, TAG, PCF("OCServer is starting..."));
   
     if (ConnectToNetwork() != 0)
    {
        OC_LOG(ERROR, TAG, PCF("Unable to connect to network"));
        return;
    }
	
    
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
     
        return;
    }
	
  
	createDoorlockResource();

    
    servo.attach(servoPin);
    
   
    
    	for(angle =0; angle<90; angle++){  //닫히기   
					servo.write(angle);
					delay(15); 
				}
}





 
// The loop function is called in an endless loop
void loop()
{
	
    if (OCProcess() != OC_STACK_OK)
    {
    
        return;
    }
    
    
	delay(1000);
	
	
    
}



void createDoorlockResource()
{
 
    OCStackResult res = OCCreateResource(&doorlock.handle,
            "core.doorlock",
            OC_RSRVD_INTERFACE_DEFAULT,
            "/iotar/doorlock",
            OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE|OC_OBSERVABLE);
   
}
