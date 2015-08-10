/*
 * raspServer.cpp
 *
 *  Created on: 2015. 8. 8.
 *      Author: hyun
 */

#include <functional>

#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;
namespace PH = std::placeholders;


// Forward declaring the entityHandler

/// This class represents a single resource named 'Light'. This resource has
/// two simple properties named 'state' and 'power'

char ipAddress[30];

class raspServer
{

public:
    /// 각각 필요한 변수들 선언 고고
    std::string m_name;
    bool m_state;
    int m_power;


    // 이 아래 변수는 필수
    std::string server_Uri;
    OCResourceHandle server_resourceHandle;
    OCRepresentation server_Rep;
    ObservationIds server_interestedObservers;


public:
    /// Constructor
    raspServer(std::string name, int p)
        :m_name(name), m_state(false), m_power(p), server_Uri("/iotar/server"),
                server_resourceHandle(nullptr) {
        // Initialize representation
        server_Rep.setUri(server_Uri);

        server_Rep.setValue("state", m_state);
        server_Rep.setValue("power", m_power);
        server_Rep.setValue("name", m_name);
    }

    /* Note that this does not need to be a member function: for classes you do not have
    access to, you can accomplish this with a free function: */

    /// This function internally calls registerResource API.
    void createResource()
    {
        //URI of the resource
        std::string resourceURI = server_Uri;
        //resource type name. In this case, it is server
        std::string resourceTypeName = "core.server";
        // resource interface.
        std::string resourceInterface = DEFAULT_INTERFACE;

        // OCResourceProperty is defined ocstack.h
        uint8_t resourceProperty;


           resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

        EntityHandler cb = std::bind(&raspServer::entityHandler, this,PH::_1);

        // This will internally create and register the resource.
        OCStackResult result = OCPlatform::registerResource(
                                    server_resourceHandle, resourceURI, resourceTypeName,
                                    resourceInterface, cb, resourceProperty);

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation was unsuccessful\n";
        }
    }



    // gets the updated representation.
    // Updates the representation with latest internal state before
    // sending out.
    OCRepresentation get()
    {

        //server_Rep.setValue("power", m_power);


        return server_Rep;
    }



    void sendIpToAndroid(char* argv){

    	if(ipAddress != NULL){
    		int sock;


    		struct sockaddr_in serv_adr;

    		sock = socket(PF_INET, SOCK_DGRAM, 0);

    		if(sock == -1){
    			std::cout << "socket() error" << std::endl;
    		}

    		memset(&serv_adr, 0, sizeof(serv_adr));
    		serv_adr.sin_family = AF_INET;
    		inet_pton(AF_INET, argv, &serv_adr.sin_addr);
    		//serv_adr.sin_addr.s_addr = inet_addr("112.108.39.255");
    		serv_adr.sin_port = htons(9100);   //9100 포트로 한다고 가정 ..

    		std::cout << argv << std::endl;

    		if(sendto(sock, ipAddress, strlen(ipAddress), 0,
    				(struct sockaddr*) &serv_adr, sizeof(serv_adr))== -1){

    			std::cout<< "app으로 ip 전송 실패" << std::endl;
    		}else{
    			std::cout<< "ip broadcast" << std::endl;
    		}



    		close(sock);



    	}else {

    		std::cout << "server의 아이피와 포트를 못 찾았습니다. " << std::endl;

    	}


    }



private:
// 이 안에 서버 get put 등의 명령어를 정의함.
OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    cout << "\tIn Server CPP entity handler:\n";
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if(request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            cout << "\t\trequestFlag : Request\n";
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());



            // If the request type is GET
            if(requestType == "GET")
            {
            	pResponse->setErrorCode(200);
            	pResponse->setResponseResult(OC_EH_OK);
         		pResponse->setResourceRepresentation(get());
            	cout << "GET 처리완료" << endl;
            	if(OC_STACK_OK == OCPlatform::sendResponse(pResponse)){
            			ehResult = OC_EH_OK;
              	}

            }
            else if(requestType == "PUT")
            {
            	//put
            }
            else if(requestType == "POST")
            {
            	//post
            }
            else if(requestType == "DELETE")
            {
                cout << "Delete request received" << endl;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
        		//observe
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }

    return ehResult;
}

};

// ChangeLightRepresentaion is an observation function,
// which notifies any changes to the resource to stack
// via notifyObservers
void foundServer(std::shared_ptr<OCResource> resource){



  	if(resource){

  		if(resource->uri() == "/iotar/server"){

  			strcpy(ipAddress, resource->host().c_str());
  			cout<< resource->host() << endl;
  			//이 호스트를 app에게 알려줘야 하는데 ...
  		}

  	}
  }

  void getServerIp(){

  	std::ostringstream requestURI;

  	requestURI << OC_RSRVD_WELL_KNOWN_URI;
  	OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundServer);

  	//라즈베리파이는 아래 로 해야함
  	// requestURI << OC_MULTICAST_DISCOVERY_URI;
  	// OCPlatform::findResource("", requestURI.str(), OC_ALL, &foundServer);
  	cout << "server IP 찾는 중...." << endl;

  }



int main(int argc, char* argv[])
{


    // Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Both,   //서버는 클라이언트 서버 둘다 해야 하므로
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        OC::QualityOfService::LowQos
    };

    OCPlatform::Configure(cfg);
    try
    {
        // Create the instance of the resource class
        // (in this case instance of class 'Light').
    	raspServer server("hyun", 100);
    	std::cout << "\nresource 생성 전 " << endl;
    	server.createResource();
    	std::cout << "resource 생성 완료 " << endl;


    	getServerIp();
    	sleep(1);
    	server.sendIpToAndroid(argv[1]);


        // A condition variable will free the mutex it is given, then do a non-
        // intensive block until 'notify' is called on it.  In this case, since we
        // don't ever call cv.notify, this should be a non-processor intensive version
        // of while(true);
        std::mutex blocker;
        std::condition_variable cv;
        std::unique_lock<std::mutex> lock(blocker);
        std::cout <<"Waiting" << std::endl;
        cv.wait(lock, []{return false;});
    }
    catch(OCException &e)
    {
        std::cout << "OCException in main : " << e.what() << endl;
    }

    // No explicit call to stop the platform.
    // When OCPlatform::destructor is invoked, internally we do platform cleanup

    return 0;
}



