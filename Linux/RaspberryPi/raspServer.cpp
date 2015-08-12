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
std::shared_ptr<OCResource> doorLock;
std::shared_ptr<OCResource> speaker;
std::shared_ptr<OCResource> hue;

class raspServer {

public:

	//명령어
	int order;

	// hue
	std::string hue_name; std::string hue_state[4]; int hue_power[4]; int hue_brightness[4]; int hue_color[4];

	// doorlock
	std::string doorlock_name; std::string doorlock_key;

	// speaker
	std::string speaker_name;


	// 이 아래 변수는 필수
	std::string server_Uri;
	OCResourceHandle server_resourceHandle;
	OCRepresentation server_Rep;
	ObservationIds server_interestedObservers;

public:
	/// Constructor
	raspServer() :
			server_Uri("/iotar/server"), server_resourceHandle(nullptr) {
		// Initialize representation
		server_Rep.setUri(server_Uri);

		hue_name = "not_linked";
		doorlock_name = "not_linked";
		speaker_name = "not_linked";

		for(int i=1; i<=3; i++){
			hue_state[i] = "off";
			hue_power[i] = 0;
			hue_brightness[i] = 0;
			hue_color[i] = 0;
		}

		get();

	}

	/* Note that this does not need to be a member function: for classes you do not have
	 access to, you can accomplish this with a free function: */

	/// This function internally calls registerResource API.
	void createResource() {
		//URI of the resource
		std::string resourceURI = server_Uri;
		//resource type name. In this case, it is server
		std::string resourceTypeName = "core.server";
		// resource interface.
		std::string resourceInterface = DEFAULT_INTERFACE;

		// OCResourceProperty is defined ocstack.h
		uint8_t resourceProperty;

		resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

		EntityHandler cb = std::bind(&raspServer::entityHandler, this, PH::_1);

		// This will internally create and register the resource.
		OCStackResult result = OCPlatform::registerResource(
				server_resourceHandle, resourceURI, resourceTypeName,
				resourceInterface, cb, resourceProperty);

		if (OC_STACK_OK != result) {
			cout << "Resource creation was unsuccessful\n";
		}
	}

	// gets the updated representation.
	// Updates the representation with latest internal state before
	// sending out.
	OCRepresentation get() {

		server_Rep.setValue("hue_name", hue_name);

		server_Rep.setValue("hue1_state", hue_state[1]);
		server_Rep.setValue("hue1_power", hue_power[1]);
		server_Rep.setValue("hue1_brightness", hue_brightness[1]);
		server_Rep.setValue("hue1_color", hue_color[1]);

		server_Rep.setValue("hue2_state", hue_state[2]);
		server_Rep.setValue("hue2_power", hue_power[2]);
		server_Rep.setValue("hue2_brightness", hue_brightness[2]);
		server_Rep.setValue("hue2_color", hue_color[2]);

		server_Rep.setValue("hue3_state", hue_state[3]);
		server_Rep.setValue("hue3_power", hue_power[3]);
		server_Rep.setValue("hue3_brightness", hue_brightness[3]);
		server_Rep.setValue("hue3_color", hue_color[3]);
//---------------------------------------------------------------------------------------
		server_Rep.setValue("doorlock_name", doorlock_name);

//---------------------------------------------------------------------------------------
		server_Rep.setValue("hue_name", hue_name);

		return server_Rep;
	}

	void sendIpToAndroid(char* argv) {

		if (ipAddress != NULL) {
			int sock;

			struct sockaddr_in serv_adr;

			sock = socket(PF_INET, SOCK_DGRAM, 0);

			if (sock == -1) {
				std::cout << "socket() error" << std::endl;
			}

			memset(&serv_adr, 0, sizeof(serv_adr));
			serv_adr.sin_family = AF_INET;
			inet_pton(AF_INET, argv, &serv_adr.sin_addr);
			//serv_adr.sin_addr.s_addr = inet_addr("112.108.39.255");
			serv_adr.sin_port = htons(9100);   //9100 포트로 한다고 가정 ..

			std::cout << argv << std::endl;

			if (sendto(sock, ipAddress, strlen(ipAddress), 0,
					(struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) {

				std::cout << "app으로 ip 전송 실패" << std::endl;
			} else {
				std::cout << "ip broadcast" << std::endl;
			}

			close(sock);

		} else {

			std::cout << "server의 아이피와 포트를 못 찾았습니다. " << std::endl;

		}

	}

private:
// 이 안에 서버 get put 등의 명령어를 정의함.
	OCEntityHandlerResult entityHandler(
			std::shared_ptr<OCResourceRequest> request) {
		std::cout << "서버에게 요청이 왔습니다." << std::endl;
		OCEntityHandlerResult ehResult = OC_EH_ERROR;
		if (request) {
			// Get the request type and request flag
			std::string requestType = request->getRequestType();
			int requestFlag = request->getRequestHandlerFlag();

			if (requestFlag & RequestHandlerFlag::RequestFlag) {
				auto pResponse = std::make_shared<OC::OCResourceResponse>();
				pResponse->setRequestHandle(request->getRequestHandle());
				pResponse->setResourceHandle(request->getResourceHandle());

				// If the request type is GET
				if (requestType == "GET") {
					pResponse->setErrorCode(200);
					pResponse->setResponseResult(OC_EH_OK);
					pResponse->setResourceRepresentation(get());
					cout << "GET 처리완료" << endl;
					if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
						ehResult = OC_EH_OK;
					}

				} else if (requestType == "PUT") {
					//put
				} else if (requestType == "POST") {
					//post
				} else if (requestType == "DELETE") {
					cout << "Delete request received" << endl;
				}
			}

			if (requestFlag & RequestHandlerFlag::ObserverFlag) {
				//observe
			}
		} else {
			std::cout << "유효하지 않은 요청" << std::endl;
		}

		return ehResult;
	}

};

// ChangeLightRepresentaion is an observation function,
// which notifies any changes to the resource to stack
// via notifyObservers
void foundServer(std::shared_ptr<OCResource> resource) {

	if (resource) {

		if (resource->uri() == "/iotar/server") {

			strcpy(ipAddress, resource->host().c_str());
			cout << "find server" << endl;
			//이 호스트를 app에게 알려줘야 하는데 ...
		}

		if (resource->uri() == "/iotar/hue"){
			hue = resource;
			cout << "find hue" << endl;
			// 휴 리소스 세팅~!!
		}

	}
}

void FindServerANDHue() {

	std::ostringstream requestURI;

	requestURI << OC_RSRVD_WELL_KNOWN_URI;
	OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundServer);

	//라즈베리파이는 아래 로 해야함
	// requestURI << OC_MULTICAST_DISCOVERY_URI;
	// OCPlatform::findResource("", requestURI.str(), OC_ALL, &foundServer);
	cout << "server & hue 찾는 중...." << endl;

}

int main(int argc, char* argv[]) {

	// Create PlatformConfig object
	PlatformConfig cfg { OC::ServiceType::InProc, OC::ModeType::Both, //서버는 클라이언트 서버 둘다 해야 하므로
			"0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
			0,         // Uses randomly available port
			OC::QualityOfService::LowQos };

	OCPlatform::Configure(cfg);
	try {
		// Create the instance of the resource class
		// (in this case instance of class 'Light').
		raspServer server;
		std::cout << "\nresource 생성 전 " << endl;
		server.createResource();
		std::cout << "resource 생성 완료 " << endl;

		FindServerANDHue();
		sleep(2);
		server.sendIpToAndroid(argv[1]);





		/*
		 //******************************************************************* 아두이노 강제로 연결하기
		 std::vector<string> resourceTypes;
		 resourceTypes.push_back("core.doorlock");

		 std::vector<string> interfaceTypes;
		 interfaceTypes.push_back("oic.if.baseline");
		 interfaceTypes.push_back("oic.if.ll");

		 doorLock = OCPlatform::constructResourceObject(아두이노 도어락 아이피 , "/iotar/doorlock" , CT_DEFAULT, true, resourceTypes, interfaceTypes);
		 speaker = OCPlatform::constructResourceObject(아두이노 스피커 아이피 , "/iotar/speaker" , CT_DEFAULT, true, resourceTypes, interfaceTypes);

		 testGet(doorLock);   //GET TEST
		 //****************************************************************
		 */

		std::mutex blocker;
		std::condition_variable cv;
		std::unique_lock<std::mutex> lock(blocker);
		std::cout << "Waiting" << std::endl;
		cv.wait(lock, [] {return false;});
	} catch (OCException &e) {
		std::cout << "OCException in main : " << e.what() << endl;
	}

	// No explicit call to stop the platform.
	// When OCPlatform::destructor is invoked, internally we do platform cleanup

	return 0;
}

/*
 void onGet(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode){

 try{
 if(eCode == OC_STACK_OK)
 {
 int power;
 string name;
 std::cout << "GET request was successful" << std::endl;
 std::cout << "Resource URI: " << rep.getUri() << std::endl;


 rep.getValue("power", power);
 rep.getValue("name", name);


 std::cout << "\tpower: " << power << std::endl;
 std::cout << "\tname: " << name << std::endl;


 }
 else
 {
 std::cout << "onGET Response error: " << eCode << std::endl;
 std::exit(-1);
 }


 }catch(std::exception e){
 std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
 }


 }

 void testGet(std::shared_ptr<OCResource> resource){

 if(resource){
 std::cout<< "get test " << std::endl;
 QueryParamsMap map;

 resource->get(map, &onGet);
 }

 }
