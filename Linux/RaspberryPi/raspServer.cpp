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

void doPutHue(OCRepresentation);
void doGetHue();

void doPutSpeaker(OCRepresentation);
void doGetSpeaker();

void doPutDoorlock(OCRepresentation);
void doGetDoorlock();

class raspServer {

public:

	//명령어
	int order;

	// hue
	std::string hue_name;
	std::string hue_state[4];
	std::string hue_power[4];
	int hue_brightness[4];
	int hue_color[4];

	// doorlock
	std::string doorlock_name;
	std::string doorlock_key;

	// speaker
	std::string speaker_name;
	std::string speaker_title;
	int speaker_time;
	int speaker_track;
	int speaker_volume;
	int speaker_state;

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

		for (int i = 1; i <= 3; i++) {

			hue_power[i] = "off";
			hue_brightness[i] = 0;
			hue_color[i] = 0;
		}

		doorlock_key = "null";

		speaker_time = 0;
		speaker_track = 0;
		speaker_volume = 0;
		speaker_state = 0;
		speaker_title = "null";

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

		//	server_Rep.setValue("hue_name", hue_name);

		//server_Rep.setValue("hue1_state", hue_state[1]);
		server_Rep.setValue("hue1_power", hue_power[1]);
		server_Rep.setValue("hue1_brightness", hue_brightness[1]);
		server_Rep.setValue("hue1_color", hue_color[1]);

		//server_Rep.setValue("hue2_state", hue_state[2]);
		server_Rep.setValue("hue2_power", hue_power[2]);
		server_Rep.setValue("hue2_brightness", hue_brightness[2]);
		server_Rep.setValue("hue2_color", hue_color[2]);

		//server_Rep.setValue("hue3_state", hue_state[3]);
		server_Rep.setValue("hue3_power", hue_power[3]);
		server_Rep.setValue("hue3_brightness", hue_brightness[3]);
		server_Rep.setValue("hue3_color", hue_color[3]);
//---------------------------------------------------------------------------------------
		server_Rep.setValue("doorlock_name", doorlock_name);

//---------------------------------------------------------------------------------------
		//server_Rep.setValue("speaker_name", speaker_name);
		//server_Rep.setValue("speaker_track", speaker_track);
		server_Rep.setValue("speaker_time", speaker_time);
		server_Rep.setValue("speaker_volume", speaker_volume);
		server_Rep.setValue("speaker_state", speaker_state);
		server_Rep.setValue("speaker_title", speaker_title);

		speaker_time = speaker_time + 2;
		return server_Rep;
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

					doGetHue();
					doGetSpeaker();     //도어락이 굳이 get 할 필요가?


					pResponse->setErrorCode(200);
					pResponse->setResponseResult(OC_EH_OK);
					pResponse->setResourceRepresentation(get());
					cout << "GET 처리완료" << endl;
					if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
						ehResult = OC_EH_OK;
					}

				} else if (requestType == "PUT") {
					//putcout << "\t\t\trequestType : PUT\n";
					int order;
					int temp;
					OCRepresentation rep = request->getResourceRepresentation();
					rep.getValue("order", order);

					if(order < 10){

					}else if(order <30){  //hue   10~29
						doPutHue(rep);
					}else if(order <40){  // speaker  30 ~ 39
						doPutSpeaker(rep);
					}else{					// doorlock  40 ~

					}
					pResponse->setErrorCode(200);
					pResponse->setResponseResult(OC_EH_OK);
					pResponse->setResourceRepresentation(get());
					if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
						ehResult = OC_EH_OK;
					}
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

raspServer server;

// ChangeLightRepresentaion is an observation function,
// which notifies any changes to the resource to stack
// via notifyObservers
void foundResource(std::shared_ptr<OCResource> resource) {

	if (resource) {


		if (resource->uri() == "/iotar/hue") {
			hue = resource;
			cout << "find hue" << endl;
			// 휴 리소스 세팅~!!
		}

		if (resource->uri() == "/iotar/speaker") {
			speaker = resource;
			cout << "find speaker" << endl;
		}

		if (resource->uri() == "/iotar/doorlock") {
			doorLock = resource;
			cout << "find doorlock" << endl;
		}

	}
}

void FindResources() {

	std::ostringstream requestURI;

	requestURI << OC_RSRVD_WELL_KNOWN_URI;
	OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundResource);

	//라즈베리파이는 아래 로 해야함
	// requestURI << OC_MULTICAST_DISCOVERY_URI;
	// OCPlatform::findResource("", requestURI.str(), OC_ALL, &foundServer);
	cout << "server & hue 찾는 중...." << endl;

}

//  do 와 on은 하나 !!
//***************************************************************************************************************** 한세트( put get 보내는 건 전역함수로 해야 됨)
void onPutHue(const HeaderOptions& headerOptions,
		const OCRepresentation& rep, const int eCode) {

	try {
		if (eCode == OC_STACK_OK) {
			std::cout << "PUT request was successful" << std::endl;

				// 풋완료후 받을 것.

		} else {
			std::cout << "onPut Response error: " << eCode << std::endl;
			std::exit(-1);
		}
	} catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
	}

}

void doPutHue(OCRepresentation rep){
	if (hue) {


				std::cout << "put to hue" << std::endl;




				hue->put(rep, QueryParamsMap(), &onPutHue);

		}else{
			std::cout << "hue not found..." << std::endl;
			FindResources();
		}


}

void onGetHue(const HeaderOptions& headerOptions,
		const OCRepresentation& rep, const int eCode){
	try {
			if (eCode == OC_STACK_OK) {
				std::cout << "GET request was successful" << std::endl;

				//휴에서 받아오는 거
				rep.getValue("hue1_power", server.hue_power[1]);

				//받고나서 트랙을 이용해 타이틀알아내기.

			} else {
				std::cout << "onGET Response error: " << eCode << std::endl;
				std::exit(-1);
			}
		} catch (std::exception& e) {
			std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
		}

}

void doGetHue(){
	if (hue) {
			std::cout << "Get to hue..." << std::endl;
			// Invoke resource's get API with the callback parameter
			hue->get(QueryParamsMap(), &onGetHue);
	}else{
		std::cout << "hue not found..." << std::endl;
		FindResources();
	}
}




//***************************************************************************************************************** 한세트( put get 보내는 건 전역함수로 해야 됨)
void onGetSpeaker(const HeaderOptions& headerOptions,
		const OCRepresentation& rep, const int eCode) {
	try {
		if (eCode == OC_STACK_OK) {
			std::cout << "GET request was successful" << std::endl;

			rep.getValue("state", server.speaker_state);
			rep.getValue("volume", server.speaker_volume);
			rep.getValue("time", server.speaker_time);
			rep.getValue("present_song", server.speaker_track);

			std::cout << "\tstate: " << server.speaker_state << std::endl;
			std::cout << "\tvol: " << server.speaker_volume << std::endl;
			std::cout << "\ttime: " << server.speaker_time << std::endl;
			std::cout << "\tsong: " << server.speaker_track << std::endl;

			//받고나서 트랙을 이용해 타이틀알아내기.

		} else {
			std::cout << "onGET Response error: " << eCode << std::endl;
			std::exit(-1);
		}
	} catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
	}
}

void doGetSpeaker() {

	if (speaker) {
		std::cout << "Get to speaker..." << std::endl;
		// Invoke resource's get API with the callback parameter
		speaker->get(QueryParamsMap(), &onGetSpeaker);
	}else{
		std::cout << "speaker not found..." << std::endl;
		FindResources();
	}

}

void onPutSpeaker(const HeaderOptions& headerOptions,
		const OCRepresentation& rep, const int eCode) {

	try {
		if (eCode == OC_STACK_OK) {
			std::cout << "PUT request was successful" << std::endl;
			rep.getValue("time", server.speaker_time);
			rep.getValue("vol", server.speaker_volume);
			rep.getValue("track", server.speaker_track);
			rep.getValue("state", server.speaker_state);

			std::cout << "time : " << server.speaker_time << std::endl;
		} else {
			std::cout << "onPut Response error: " << eCode << std::endl;
			std::exit(-1);
		}
	} catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
	}

}

void doPutSpeaker(OCRepresentation rep) {

	if (speaker) {

		std::cout << "put to speaker" << std::endl;


		speaker->put(rep, QueryParamsMap(), &onPutSpeaker);

	}else{
		std::cout << "speaker not found..." << std::endl;
		FindResources();
	}
}

//***************************************************************************************************************** 한세트
void onGetDoorlock(const HeaderOptions& headerOptions,
		const OCRepresentation& rep, const int eCode) {
	try {
		if (eCode == OC_STACK_OK) {
			std::cout << "GET request was successful" << std::endl;


			//get 에서 가져올게 뭔지

		} else {
			std::cout << "onGET Response error: " << eCode << std::endl;
			std::exit(-1);
		}
	} catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
	}
}

void doGetDoorlock() {

	if (doorLock) {
		std::cout << "Get to speaker..." << std::endl;
		// Invoke resource's get API with the callback parameter
		doorLock->get(QueryParamsMap(), &onGetDoorlock);
	}else{
		std::cout << "speaker not found..." << std::endl;
		FindResources();
	}

}

void onPutDoorlock(const HeaderOptions& headerOptions,
		const OCRepresentation& rep, const int eCode) {

	try {
		if (eCode == OC_STACK_OK) {
			std::cout << "PUT request was successful" << std::endl;


		} else {
			std::cout << "onPut Response error: " << eCode << std::endl;
			std::exit(-1);
		}
	} catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
	}

}

void doPutDoorlock(OCRepresentation rep) {

	if (doorLock) {

		std::cout << "put to speaker" << std::endl;

		doorLock->put(rep, QueryParamsMap(), &onPutDoorlock);

	}else{
		std::cout << "speaker not found..." << std::endl;
		FindResources();
	}
}



//***************************************************************************************************************** 한세트
int main(int argc, char* argv[]) {

	// Create PlatformConfig object
	PlatformConfig cfg { OC::ServiceType::InProc, OC::ModeType::Both, //서버는 클라이언트 서버 둘다 해야 하므로
			"0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
			0,         // Uses randomly available port
			OC::QualityOfService::LowQos };

	OCPlatform::Configure(cfg);
	try {


		std::cout << "\nresource 생성 전 " << endl;
		server.createResource();
		std::cout << "resource 생성 완료 " << endl;

		FindResources();


		std::mutex blocker;
		std::condition_variable cv;
		std::unique_lock<std::mutex> lock(blocker);
		std::cout << "Waiting" << std::endl;
		cv.wait(lock, [] {return false;});
	} catch (OCException &e) {
		std::cout << "OCException in main : " << e.what() << endl;
	}


	return 0;
}
