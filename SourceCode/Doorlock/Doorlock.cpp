// Do not remove the include below
#include "Arduino.h"

#include "logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include <string.h>


//#ifdef ARDUINOWIFI
// Arduino WiFi Shield
#include <SPI.h>
#include <Servo.h>
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
	//OC_LOG(DEBUG, TAG, PCF("ConnectToNetwork is starting..."));
    // Note: ****Update the MAC address here with your shield's MAC address****

	  
	  if(Ethernet.begin(mac)== 0){
	  
	  
		//  OC_LOG(DEBUG, TAG, PCF("Failed to configure Ethernet using DHCP"));
   
  
  // give the Ethernet shield a second to initialize:
}
  delay(1000);
    

  IPAddress  ip = Ethernet.localIP();
   OC_LOG_V(INFO, TAG, "IP Address:  %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return 0;
}
//#endif //ARDUINOWIFI

// This is the entity handler for the registered resource.
// This is invoked by OCStack whenever it recevies a request for this resource.


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
    // Add your initialization code here
    // Note : This will initialize Serial port on Arduino at 115200 bauds
   // OC_LOG_INIT();
   
    Serial.begin(115200);
    Serial1.begin(115200);
	
    OC_LOG(DEBUG, TAG, PCF("OCServer is starting..."));
//    OC_LOG(DEBUG, TAG, PCF("OCServer is starting..."));
	
    // Connect to Ethernet or WiFi network
   
     if (ConnectToNetwork() != 0)
    {
        OC_LOG(ERROR, TAG, PCF("Unable to connect to network"));
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

    
    servo.attach(servoPin);
    
    OC_LOG(DEBUG, TAG, PCF("소라 놋북 터진 날 15.07.31 ..."));
    
    	for(angle =0; angle<90; angle++){  //닫히기   
					servo.write(angle);
					delay(15); 
				}
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
    
    
	delay(1000);
	
	
    
}



void createLightResource()
{
 
    OCStackResult res = OCCreateResource(&doorlock.handle,
            "core.doorlock",
            OC_RSRVD_INTERFACE_DEFAULT,
            "/iotar/doorlock",
            OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE|OC_OBSERVABLE);
   OC_LOG_V(INFO, TAG, "Created Light resource with result: %s", getResult(res));
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

