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

#include <fstream>
#include <cstdlib>
#include <thread>

#include <wiringPi.h>

#include "OCPlatform.h"
#include "OCApi.h"

#define MAX_SONG 12

#define SW1 0 // BCM_GPIO 17
#define SW2 1 // BCM_GPIO 18
#define SW3 2 // BCM_GPIO 27
#define SW4 3 // BCM_GPIO 22

using namespace OC;
using namespace std;
namespace PH = std::placeholders;

char ipAddress[30];
std::shared_ptr<OCResource> doorLock;
std::shared_ptr<OCResource> speaker;
std::shared_ptr<OCResource> hue;

void doPutHue(OCRepresentation);
void doGetHue();
void doPostHue(OCRepresentation);
void doDeleteHue();

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
    double hue_color_x[4];
    double hue_color_y[4];
    // doorlock
    std::string doorlock_name;
    std::string doorlock_key;

    bool hue1_group;
 bool hue2_group;
 bool hue3_group;
    OCRepresentation Hue_rep;
bool huePutFirst;

    // speaker
    std::string speaker_name;
    char speaker_title[30];
    char speaker_singer[30];
    char speaker_lyrics[100][100];
    int speaker_lyrics_time[100];
    int speaker_line;
    int speaker_time;
    int song_time;
    int speaker_track;
    int speaker_volume;
    int speaker_state;


    int speaker_makelyrics;

    std::string speaker_list_title = "너와 나/기대해/Rain Drop/Love day/위 아래/심쿵해/뱅뱅뱅/스폰서/레옹/맙소사/Lion heart/위잉위잉";
    std::string speaker_list_singer = "타루/걸스데이/아이유/양요섭,정은지/EXID/AOA/빅뱅/하하&자이언티/아이유&박명수/황태지/소녀시대/혁오";

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

            hue_power[i] = "false";
            hue_brightness[i] = 0;
            hue_color_x[i] = 0;
            hue_color_y[i] = 0;
        }

	 hue1_group = false;
 hue2_group = false;
 hue3_group = false;

	huePutFirst = false;

        doorlock_key = "1234";

        speaker_time = 0;
        speaker_track = 0;
        speaker_volume = 0;
        speaker_state = 0;
        speaker_makelyrics = 0;
        song_time=0;

        get();

    }

    void createResource() {

        std::string resourceURI = server_Uri;
        std::string resourceTypeName = "core.server";
        std::string resourceInterface = DEFAULT_INTERFACE;

        uint8_t resourceProperty;

        resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

        EntityHandler cb = std::bind(&raspServer::entityHandler, this, PH::_1);

        OCStackResult result = OCPlatform::registerResource(
                server_resourceHandle, resourceURI, resourceTypeName,
                resourceInterface, cb, resourceProperty);

        if (OC_STACK_OK != result) {
            cout << "server creation was unsuccessful\n";
        }
    }

    void makeLyrics(int a) {
        char temp[10];
        speaker_makelyrics = a;
        speaker_line = 0;
        ifstream fin;
        std::string song = "/home/pi/track";

        song.append(to_string(a));
        fin.open(song.c_str());
        if (!fin.good())
            return;

        cout << "가사가 뭐임? : " << a << endl;

        fin.getline(speaker_title, 30);
        fin.getline(speaker_singer, 30);
        fin.getline(temp, 10);
        song_time = atoi(temp);

        while (!fin.eof()) {
            fin.getline(temp, 10);
            speaker_lyrics_time[speaker_line] = atoi(temp);
            fin.getline(speaker_lyrics[speaker_line++], 100);
        }

        fin.close();
    }

    int getLyrics(int time) {

        for (int i = 1; i < speaker_line; i++) {

            if (speaker_lyrics_time[i] >= time) {
                return i;
            }

        }

        return -1;

    }

    OCRepresentation get() {
/*
        //    server_Rep.setValue("hue_name", hue_name);

        //server_Rep.setValue("hue1_state", hue_state[1]);
        server_Rep.setValue("hue1_power", hue_power[1]);
        server_Rep.setValue("hue1_brightness", hue_brightness[1]);
        server_Rep.setValue("hue1_color_x", hue_color_x[1]);
        server_Rep.setValue("hue1_color_y", hue_color_y[1]);

        //server_Rep.setValue("hue2_state", hue_state[2]);
        server_Rep.setValue("hue2_power", hue_power[2]);
        server_Rep.setValue("hue2_brightness", hue_brightness[2]);
        server_Rep.setValue("hue1_color_x", hue_color_x[2]);
        server_Rep.setValue("hue1_color_y", hue_color_y[2]);

        //server_Rep.setValue("hue3_state", hue_state[3]);
        server_Rep.setValue("hue3_power", hue_power[3]);
        server_Rep.setValue("hue3_brightness", hue_brightness[3]);
        server_Rep.setValue("hue1_color_x", hue_color_x[3]);
        server_Rep.setValue("hue1_color_y", hue_color_y[3]);
*/
	 server_Rep.setValue("hue1_power", hue_power[1]);
	 server_Rep.setValue("hue2_power", hue_power[2]);
	 server_Rep.setValue("hue3_power", hue_power[3]);
	//cout <<"wwwwwwwwwwwwwwwwwww :                " << hue_power[1] << " " << hue_power[2] << " " << hue_power[3] << endl;
//---------------------------------------------------------------------------------------
        server_Rep.setValue("doorlock_key", doorlock_key);

//---------------------------------------------------------------------------------------
        //server_Rep.setValue("speaker_name", speaker_name);
        server_Rep.setValue("speaker_track", speaker_track);

        //server_Rep.setValue("speaker_time", speaker_time);
        server_Rep.setValue("speaker_volume", speaker_volume);
        server_Rep.setValue("speaker_state", speaker_state);
//
        server_Rep.setValue("speaker_titles", speaker_list_title);
        server_Rep.setValue("speaker_singers", speaker_list_singer);

        int line = getLyrics(speaker_time);
        std::string temp;
        if (line != -1) {
            cout << "\t\t" << speaker_lyrics[line - 1] << endl;
            cout << "\t\t" << speaker_lyrics[line] << endl;
            std::string str(speaker_lyrics[line - 1]);
            std::string str1(speaker_lyrics[line]);

            server_Rep.setValue("speaker_lyrics1", str);
            server_Rep.setValue("speaker_lyrics2", str1);
        } else {

        }

//--------------------------------------------------------------------------------------
        return server_Rep;
    }

private:
// 이 안에 서버 get put 등의 명령어를 정의함.
    OCEntityHandlerResult entityHandler(
            std::shared_ptr<OCResourceRequest> request) {
        std::cout << "서버에게 요청이 왔습니다." << std::endl;
        OCEntityHandlerResult ehResult = OC_EH_ERROR;
        if (request) {
            std::string requestType = request->getRequestType();
            int requestFlag = request->getRequestHandlerFlag();

            if (requestFlag & RequestHandlerFlag::RequestFlag) {
                auto pResponse = std::make_shared<OC::OCResourceResponse>();
                pResponse->setRequestHandle(request->getRequestHandle());
                pResponse->setResourceHandle(request->getResourceHandle());

                if (requestType == "GET") {

                   // doGetHue();
                    doGetSpeaker();     //도어락이 굳이 get 할 필요가?

                    pResponse->setErrorCode(200);
                    pResponse->setResponseResult(OC_EH_OK);
                    pResponse->setResourceRepresentation(get());
                    if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
                        ehResult = OC_EH_OK;
                        cout << "GET 처리완료" << endl;

                    }

                } else if (requestType == "PUT") {
                    //put
                    cout << "\t\t\trequestType : PUT\n";
                    int order;

                    OCRepresentation rep = request->getResourceRepresentation();
                    rep.getValue("order", order);

                     if (order < 30) {  //hue   10~29
			Hue_rep = rep;
			

			if(order != 11){
			rep.getValue("hue1_group", hue1_group);
			rep.getValue("hue2_group", hue2_group);
			rep.getValue("hue3_group", hue3_group);
			
			rep.getValue("hue1_power", hue_power[1]);
			rep.getValue("hue2_power", hue_power[2]);
			rep.getValue("hue3_power", hue_power[3]);
			}
			//cout << hue_power[1] << hue_power[2] << hue_power[3] << endl;
			huePutFirst = true;
			
			
			

                        doPutHue(rep);
                    } else if (order < 40) {  // speaker  30 ~ 39
                        if(order == 37){
				if(speaker_state == 1){
					speaker_state = 2;
				}else{
					speaker_state = 1;
				}
			}
			doPutSpeaker(rep);
                    } else {                    // doorlock  40 ~
			if(order == 40){
				rep.getValue("doorlock_key", doorlock_key);
			}else if(order == 41){
				cout << "open open open open open open open open" << endl;
				doPutDoorlock(rep);
			}
                    }
                    pResponse->setErrorCode(200);
                    pResponse->setResponseResult(OC_EH_OK);
                    pResponse->setResourceRepresentation(get());
                    if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
                        ehResult = OC_EH_OK;
                    }
                } else if (requestType == "POST") {
                    OCRepresentation rep = request->getResourceRepresentation();
                    //doPostHue(rep);
                    //----------------------------------------------------------------------------------
                } else if (requestType == "DELETE") {
                    cout << "Delete request received" << endl;
                    //doDeleteHue();
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

    cout << "리소스들을 다시 찾는 중...." << endl;

}

//  do 와 on은 하나 !!
//***************************************************************************************************************** 한세트( put get 보내는 건 전역함수로 해야 됨)

void onDeleteHue(const HeaderOptions& headerOptions,
        const int eCode){
    try {
            if (eCode == OC_STACK_OK) {
                std::cout << "Delete request was successful" << std::endl;

                // 풋완료후 받을 것.

            } else {
                std::cout << "onDelete Response error: " << eCode << std::endl;
                //std::exit(-1);
            }
        } catch (std::exception& e) {
            std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
        }

}
void doDeleteHue(){
    if (hue) {

            std::cout << "delete to hue" << std::endl;

            hue->deleteResource(&onDeleteHue);

        } else {
            std::cout << "hue not found..." << std::endl;
            FindResources();
        }
}

void onPostHue(const HeaderOptions& headerOptions, const OCRepresentation& rep,
        const int eCode) {

    try {
        if (eCode == OC_STACK_OK) {
            std::cout << "Post request was successful" << std::endl;

            // 풋완료후 받을 것.

        } else {
            std::cout << "onPost Response error: " << eCode << std::endl;
            //std::exit(-1);
        }
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
    }

}

void doPostHue(OCRepresentation rep) {

    if (hue) {

        std::cout << "post to hue" << std::endl;

        hue->put(rep, QueryParamsMap(), &onPostHue);

    } else {
        std::cout << "hue not found..." << std::endl;
        FindResources();
    }

}

void onPutHue(const HeaderOptions& headerOptions, const OCRepresentation& rep,
        const int eCode) {

    try {
        if (eCode == OC_STACK_OK) {
            std::cout << "PUT request was successful" << std::endl;

            // 풋완료후 받을 것.

        } else {
            std::cout << "onPut Response error: " << eCode << std::endl;
            //std::exit(-1);
        }
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
    }

}

void doPutHue(OCRepresentation rep) {

    if (hue) {

        std::cout << "put to hue" << std::endl;

        hue->put(rep, QueryParamsMap(), &onPutHue);

    } else {
        std::cout << "hue not found..." << std::endl;
        FindResources();
    }

}

void onGetHue(const HeaderOptions& headerOptions, const OCRepresentation& rep,
        const int eCode) {

    try {
        if (eCode == OC_STACK_OK) {
            std::cout << "GET request was successful" << std::endl;

            //휴에서 받아오는 거
            rep.getValue("hue1_power", server.hue_power[1]);

            //받고나서 트랙을 이용해 타이틀알아내기.

        } else {
            std::cout << "onGET Response error: " << eCode << std::endl;
            //std::exit(-1);
        }
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
    }

}

void doGetHue() {

    if (hue) {
        std::cout << "Get to hue..." << std::endl;

        hue->get(QueryParamsMap(), &onGetHue);
    } else {
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

            if (server.speaker_makelyrics != server.speaker_track)
                server.makeLyrics(server.speaker_track); // 가사 뽑아오기.

            //받고나서 트랙을 이용해 타이틀알아내기.

        } else {
            std::cout << "onGET Response error: " << eCode << std::endl;
            //std::exit(-1);
        }
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
    }
}

void doGetSpeaker() {

    if (speaker) {
        std::cout << "Get to speaker..." << std::endl;

        speaker->get(QueryParamsMap(), &onGetSpeaker);
    } else {
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
            //std::exit(-1);
        }
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
    }

}

void doPutSpeaker(OCRepresentation rep) {

    if (speaker) {

        std::cout << "put to speaker" << std::endl;

        speaker->put(rep, QueryParamsMap(), &onPutSpeaker);

    } else {
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
            //std::exit(-1);
        }
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
    }
}

void doGetDoorlock() {

    if (doorLock) {
        std::cout << "Get to doorlcok..." << std::endl;

        doorLock->get(QueryParamsMap(), &onGetDoorlock);
    } else {
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
            //std::exit(-1);
        }
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
    }

}

void doPutDoorlock(OCRepresentation rep) {

    if (doorLock) {

        std::cout << "put to doorlcok" << std::endl;

        doorLock->put(rep, QueryParamsMap(), &onPutDoorlock);

    } else {
        std::cout << "speaker not found..." << std::endl;
        FindResources();
    }
}

//***************************************************************************************************************** 한세트

void ThreadGetSpeaker(){

    while(1){
        doGetSpeaker();

        sleep(1);

	if(server.speaker_time > server.song_time+3){
		OCRepresentation rep;
		rep.setValue("order", 30);
		if(server.speaker_track == MAX_SONG){
			rep.setValue("temp", 1);
		}else{
			rep.setValue("temp", server.speaker_track+1);
		}
		
		doPutSpeaker(rep);
		sleep(5);
	}
    }
}


int main(int argc, char* argv[]) {

	 if (wiringPiSetup() == -1)
  		return 1 ;

    PlatformConfig cfg { OC::ServiceType::InProc, OC::ModeType::Both, //서버는 클라이언트 서버 둘다 해야 하므로
            "0.0.0.0", 0, OC::QualityOfService::LowQos };

    OCPlatform::Configure(cfg);
    try {

        std::cout << "\nserver 생성 전 " << endl;
        server.createResource();
        std::cout << "server 생성 완료 " << endl;

        FindResources();

        std::string a;

        //server.makeLyrics(2);
        std::thread first(ThreadGetSpeaker);

	pinMode(SW1, INPUT);
	pinMode(SW2, INPUT);
	pinMode(SW3, INPUT);
	pinMode(SW4, INPUT);

        while (1) {

		delay(200);
		if(server.huePutFirst == true){

		//cout << digitalRead(SW1) << endl;
		//cout << digitalRead(SW2) << endl;
		//cout << digitalRead(SW3) << endl;
		//cout << digitalRead(SW4) << endl;

		if(digitalRead(SW1) == 1){
			delay(500);
			if(digitalRead(SW1) == 1){
				cout << "push sw 1 " << endl;
				if(server.hue_power[1] == "true"){
					server.hue_power[1] = "false";
				}else{
					server.hue_power[1] = "true";
				}

				if(server.hue1_group){
					if(server.hue2_group){
						server.hue_power[2] = server.hue_power[1];
					}

					if(server.hue3_group){
						server.hue_power[3] = server.hue_power[1];
					}
				}

				server.Hue_rep.setValue("hue1_power", server.hue_power[1]);
				server.Hue_rep.setValue("hue2_power", server.hue_power[2]);
				server.Hue_rep.setValue("hue3_power", server.hue_power[3]);
				doPutHue(server.Hue_rep);
				delay(1000);
			}

		}  else if(digitalRead(SW2) == 1){
			delay(500);
			if(digitalRead(SW2) == 1){
				cout << "push sw 2 " << endl;
				if(server.hue_power[2] == "true"){
					server.hue_power[2] = "false";
				}else{
					server.hue_power[2] = "true";
				}

				if(server.hue2_group){
					if(server.hue1_group){
						server.hue_power[1] = server.hue_power[2];
					}

					if(server.hue3_group){
						server.hue_power[3] = server.hue_power[2];
					}
				}

				server.Hue_rep.setValue("hue1_power", server.hue_power[1]);
				server.Hue_rep.setValue("hue2_power", server.hue_power[2]);
				server.Hue_rep.setValue("hue3_power", server.hue_power[3]);
				doPutHue(server.Hue_rep);
				delay(1000);
			}

		}  else if(digitalRead(SW3) == 1){
			delay(500);
			if(digitalRead(SW3) == 1){
				cout << "push sw 3 " << endl;
				if(server.hue_power[3] == "true"){
					server.hue_power[3] = "false";
				}else{
					server.hue_power[3] = "true";
				}

				if(server.hue3_group){
					if(server.hue1_group){
						server.hue_power[1] = server.hue_power[3];
					}

					if(server.hue2_group){
						server.hue_power[2] = server.hue_power[3];
					}
				}

				server.Hue_rep.setValue("hue1_power", server.hue_power[1]);
				server.Hue_rep.setValue("hue2_power", server.hue_power[2]);
				server.Hue_rep.setValue("hue3_power", server.hue_power[3]);
				doPutHue(server.Hue_rep);
				delay(1000);
			}

		}   else if(digitalRead(SW4) == 1){
			delay(500);
			if(digitalRead(SW4) == 1){
				cout << "push sw 4 " << endl;
				FindResources();
				
				delay(1000);
			}

		}          
		}
		

        }

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
 