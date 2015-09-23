//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

///
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server.
///
#include <functional>
#include <pthread.h>
#include <mutex>
#include <condition_variable>

#include <cpprest/base_uri.h>
#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>                       // JSON library
#include <cpprest/uri.h>                        // URI library
#include <cpprest/containerstream.h>            // Async streams backed by STL containers
#include <cpprest/interopstream.h>              // Bridges for integrating Async streams with STL and WinRT streams
#include <cpprest/rawptrstream.h>               // Async streams backed by raw pointer to memory
#include <cpprest/producerconsumerstream.h>     // Async streams for producer consumer scenarios
#include <cpprest/filestream.h>

#include <cpprest/json.h>
#include <cpprest/uri.h>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;
using namespace concurrency::streams;
// Asynchronous streams
using namespace utility;
// Common utilities like string conversions
using namespace web;
// Common features like URIs.
using namespace web::http;
// Common HTTP functionality
using namespace web::http::client;
// HTTP client features
using namespace concurrency::streams;
// Asynchronous streams
using namespace web::http::experimental::listener;
// HTTP server
using namespace web::json;
namespace PH = std::placeholders;

int gObservation = 0;

// Specifies where to notify all observers or list of observers
// false: notifies all observers
// true: notifies list of observers
bool isListOfObservers = false;

// Specifies secure or non-secure
// false: non-secure resource
// true: secure resource
bool isSecure = false;

/// Specifies whether Entity handler is going to do slow response or not
bool isSlowResponse = false;

// Forward declaring the entityHandler

/// This class represents a single resource named 'lightResource'. This resource has
/// two simple properties named 'state' and 'power'

class HueResource {

public:
	/// Access this property from a TB client
	std::string m_power[5];
	int m_brightness[5];
	double m_color_x[5];
	double m_color_y[5];
	std::string m_name;
	std::string m_lightUri;
	std::string ip, username;
	std::string m_group[5];
	int m_group_size;bool firstaccess, hf1, hf2, hf3;
	OCResourceHandle m_resourceHandle;
	OCRepresentation m_lightRep;
	ObservationIds m_interestedObservers;

public:
	/// Constructor
	HueResource() :
			m_name("iotar's hue"), m_lightUri("/iotar/hue"), m_resourceHandle(
					nullptr), firstaccess(false), hf1(false), hf2(false), hf3(
			false), m_group_size(0) {
		// Initialize representation
		for (int i = 1; i <= 3; ++i) {
			m_power[i] = "false";
			m_brightness[i] = 0;
			m_color_x[i] = 0;
			m_color_y[i] = 0;
			m_group[i] = "";
		}
		m_lightRep.setValue("hue1_brightness", m_brightness[1]);
		m_lightRep.setValue("hue2_brightness", m_brightness[2]);
		m_lightRep.setValue("hue3_brightness", m_brightness[3]);
		m_lightRep.setValue("hue1_color_x", m_color_x[1]);
		m_lightRep.setValue("hue1_color_y", m_color_y[1]);
		m_lightRep.setValue("hue2_color_x", m_color_x[2]);
		m_lightRep.setValue("hue2_color_y", m_color_y[2]);
		m_lightRep.setValue("hue3_color_y", m_color_x[3]);
		m_lightRep.setValue("hue3_color_y", m_color_y[3]);
		m_lightRep.setValue("hue1_power", m_power[1]);
		m_lightRep.setValue("hue2_power", m_power[2]);
		m_lightRep.setValue("hue3_power", m_power[3]);
	}

	/* Note that this does not need to be a member function: for classes you do not have
	 access to, you can accomplish this with a free function: */

	/// This function internally calls registerResource API.
	void createResource() {
		//URI of the resource
		std::string resourceURI = "/iotar/hue";
		//resource type name. In this case, it is light
		std::string resourceTypeName = "core.light";
		// resource interface.
		std::string resourceInterface = DEFAULT_INTERFACE;

		// OCResourceProperty is defined ocstack.h
		uint8_t resourceProperty;

		resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

		EntityHandler cb = std::bind(&HueResource::entityHandler, this, PH::_1);

		// This will internally create and register the resource.
		OCStackResult result = OCPlatform::registerResource(m_resourceHandle,
				resourceURI, resourceTypeName, resourceInterface, cb,
				resourceProperty);

		if (OC_STACK_OK != result) {
			cout << "Resource creation was unsuccessful\n";
		}
	}

	OCResourceHandle getHandle() {
		return m_resourceHandle;
	}

	// Puts representation.
	// Gets values from the representation and
	// updates the internal state
	void put(OCRepresentation& rep) {
		try {
			rep.getValue("firstaccess", firstaccess);
			cout << "put() firstAccess?(true=1, false=0) " << firstaccess
					<< endl;
			if (firstaccess) {
				rep.getValue("hue_ip", ip);
				rep.getValue("hue_username", username);
			} else {
				rep.getValue("hue1_brightness", m_brightness[1]);
				rep.getValue("hue2_brightness", m_brightness[2]);
				rep.getValue("hue3_brightness", m_brightness[3]);
				rep.getValue("hue1_color_x", m_color_x[1]);
				rep.getValue("hue1_color_y", m_color_y[1]);
				rep.getValue("hue2_color_x", m_color_x[2]);
				rep.getValue("hue2_color_y", m_color_y[2]);
				rep.getValue("hue3_color_x", m_color_x[3]);
				rep.getValue("hue3_color_y", m_color_y[3]);
				rep.getValue("hue1_power", m_power[1]);
				rep.getValue("hue2_power", m_power[2]);
				rep.getValue("hue3_power", m_power[3]);

				GetPutJson(U("http://" + ip + "/api/" + username + "/lights"));
				cout << "getPutJson end" << endl;
			}
		} catch (exception& e) {
			cout << e.what() << endl;
		}
	}

	void GetPutJson(std::string p_sUrl, std::string p_sQueryPath = U(""),
			std::vector<std::pair<std::string, std::string>>* p_pvQuery =
					nullptr) {
		web::json::value vJson;
		std::string sBody;

		for (int i = 1; i <= 3; ++i) {

			json::value postData;

			string requestBody = "{\"on\":";
			requestBody.append(m_power[i]).append(", \"xy\": [").append(
					to_string(m_color_x[i])).append(", ").append(
					to_string(m_color_y[i])).append("], \"bri\" :").append(
					to_string(m_brightness[i])).append("}");

	

			http_client client(
					U(
							"http://" + ip + "/api/" + username + "/lights/"
									+ to_string(i) + "/state/"));

			pplx::task<web::http::http_response> requestTask = client.request(
					methods::PUT, "", requestBody, "application/json");

			try {
				requestTask.wait();
			} catch (const std::exception &e) {
				printf("Error exception:%s\n", e.what());
			}

		}
	}

	// Post representation.
	// Post can create new resource or simply act like put.
	// Gets values from the representation and
	// updates the internal state
	void post(OCRepresentation& rep) {
		try {
			cout << "post()" << endl;

			rep.getValue("hf1", hf1);
			rep.getValue("hf2", hf2);
			rep.getValue("hf3", hf3);
			cout << "1" << endl;
			if (hf1 || hf2 || hf3) {
				http_client client(
						U("http://" + ip + "/api/" + username + "/groups"));
				cout << "2" << endl;
				json::value postData;
				string requestBody = "{\"lights\":";
				if (hf1 && hf2 && hf3)
					requestBody.append(" [\"").append(to_string(1)).append(
							"\", \"").append(to_string(2)).append("\", \"").append(
							to_string(3)).append("\"] }");
				else if (hf1 && hf2)
					requestBody.append(" [\"").append(to_string(1)).append(
							"\", \"").append(to_string(2)).append("\"] }");
				else if (hf1 && hf3)
					requestBody.append(" [\"").append(to_string(1)).append(
							"\", \"").append(to_string(3)).append("\"] }");
				else if (hf2 && hf3)
					requestBody.append(" [\"").append(to_string(2)).append(
							"\", \"").append(to_string(3)).append("\"] }");
				cout << requestBody << endl;
				cout << "5" << endl;
				pplx::task<web::http::http_response> requestTask =
						client.request(methods::POST, "", requestBody,
								"application/json");
				cout << "6" << endl;
				try {
					requestTask.wait();
				} catch (const std::exception &e) {
					printf("Error exception:%s\n", e.what());
				}
			}

		} catch (exception& e) {
			cout << e.what() << endl;
		}
	}

	web::uri_builder GetBuilder(std::string p_sQueryPath,
			std::vector<std::pair<std::string, std::string>>* p_pvQuery) {
		web::uri_builder builder;
		if (!p_sQueryPath.empty()) {
			builder.set_path(p_sQueryPath);
			if (!p_pvQuery->empty()) {
				for (std::pair<std::string, std::string> pQuery : (*p_pvQuery)) {
					builder.append_query(pQuery.first, pQuery.second);
				}
			}
		}

		return builder;
	}

	web::json::value GetJson(std::string p_sUrl,
			std::string p_sQueryPath = U(""),
			std::vector<std::pair<std::string, std::string>>* p_pvQuery =
					nullptr) {
		web::json::value vJson;
		std::string sBody;
		http_client client(p_sUrl);
		web::uri_builder builder = GetBuilder(p_sQueryPath, p_pvQuery);

		pplx::task<void> requestTask = client.request(web::http::methods::GET,
				builder.to_string()).then(
				[&](web::http::http_response response) {
					return response.extract_json();
				}).then([&](pplx::task<web::json::value> previousTask) {
			try
			{
				vJson = (web::json::value)previousTask.get();
			}
			catch (const web::http::http_exception& e)
			{
				printf("Error exception:%s\n", e.what());
			}
		});

		try {
			requestTask.wait();
		} catch (const std::exception &e) {
			printf("Error exception:%s\n", e.what());
		}
		return vJson;
	}

	// gets the updated representation.
	// Updates the representation with latest internal state before
	// sending out.
	OCRepresentation get() {

		// Create http_client to send the request.
		cout << "ip : " << ip << ",  username : " << username << endl;
		web::json::value v = GetJson(U("http://" + ip + "/api/" + username));

		m_power[1] =
				v.at(U("lights")).at(U("1")).at(U("state")).at(U("on")).to_string();
		cout << m_power[1] << endl;
		m_power[2] =
				v.at(U("lights")).at(U("2")).at(U("state")).at(U("on")).to_string();
		cout << m_power[2] << endl;
		m_power[3] =
				v.at(U("lights")).at(U("3")).at(U("state")).at(U("on")).to_string();
		cout << m_power[3] << endl;
		m_brightness[1] = v.at(U("lights")).at(U("1")).at(U("state")).at(
				U("bri")).as_integer();
		cout << m_brightness[1] << endl;
		m_brightness[2] = v.at(U("lights")).at(U("2")).at(U("state")).at(
				U("bri")).as_integer();
		cout << m_brightness[2] << endl;
		m_brightness[3] = v.at(U("lights")).at(U("3")).at(U("state")).at(
				U("bri")).as_integer();
		cout << m_brightness[3] << endl;
		m_color_x[1] =
				v.at(U("lights")).at(U("1")).at(U("state")).at(U("xy")).as_array().at(0).as_double();
		cout << m_color_x[1] << endl;
		m_color_y[1] =
				v.at(U("lights")).at(U("1")).at(U("state")).at(U("xy")).as_array().at(1).as_double();
		cout << m_color_y[1] << endl;
		m_color_x[2] =
				v.at(U("lights")).at(U("2")).at(U("state")).at(U("xy")).as_array().at(0).as_double();
		cout << m_color_x[2] << endl;
		m_color_y[2] =
				v.at(U("lights")).at(U("2")).at(U("state")).at(U("xy")).as_array().at(1).as_double();
		cout << m_color_y[2] << endl;
		m_color_x[3] =
				v.at(U("lights")).at(U("3")).at(U("state")).at(U("xy")).as_array().at(0).as_double();
		cout << m_color_x[3] << endl;
		m_color_y[3] =
				v.at(U("lights")).at(U("3")).at(U("state")).at(U("xy")).as_array().at(1).as_double();
		cout << m_color_y[3] << endl;

		if (firstaccess) {
			if (v.at(U("groups")).size() != 0) {
				json::array& groupArr = v.at(U("groups")).at(U("1")).at(
						U("lights")).as_array();
				int gSize = groupArr.size();
				m_group_size = 0;
				for (int i = 0; i < gSize; i++) {
					m_group[i + 1] = groupArr.at(i).to_string();
					m_lightRep.setValue("hue" + to_string(i + 1) + "_group",
							m_group[i + 1]);
					cout << "for" << endl;
					m_group_size++;
				}
			}
			m_lightRep.setValue("hue_group_size", m_group_size);
		}

		m_lightRep.setValue("hue1_brightness", m_brightness[1]);
		m_lightRep.setValue("hue2_brightness", m_brightness[2]);
		m_lightRep.setValue("hue3_brightness", m_brightness[3]);
		m_lightRep.setValue("hue1_color_x", m_color_x[1]);
		m_lightRep.setValue("hue1_color_y", m_color_y[1]);
		m_lightRep.setValue("hue2_color_x", m_color_x[2]);
		m_lightRep.setValue("hue2_color_y", m_color_y[2]);
		m_lightRep.setValue("hue3_color_x", m_color_x[3]);
		m_lightRep.setValue("hue3_color_y", m_color_y[3]);
		m_lightRep.setValue("hue1_power", m_power[1]);
		m_lightRep.setValue("hue2_power", m_power[2]);
		m_lightRep.setValue("hue3_power", m_power[3]);
		cout << "END!!!!" << endl;
		return m_lightRep;
	}

	void addType(const std::string& type) const {
		OCStackResult result = OCPlatform::bindTypeToResource(m_resourceHandle,
				type);
		if (OC_STACK_OK != result) {
			cout << "Binding TypeName to Resource was unsuccessful\n";
		}
	}

	void addInterface(const std::string& interface) const {
		OCStackResult result = OCPlatform::bindInterfaceToResource(
				m_resourceHandle, interface);
		if (OC_STACK_OK != result) {
			cout << "Binding TypeName to Resource was unsuccessful\n";
		}
	}

	void deleteGroup() {
		cout << "deleteGrou() Start" << endl;

		web::json::value v = GetJson(U("http://" + ip + "/api/" + username));
		cout << "0" << endl;
		if (v.at(U("groups")).size() != 0) {
			cout << "1" << endl;
			http_client client(
					U("http://" + ip + "/api/" + username + "/groups/1"));
			cout << "2" << endl;
			pplx::task<web::http::http_response> requestTask = client.request(
					methods::DEL);
			cout << "3" << endl;
			try {
				requestTask.wait();
			} catch (const std::exception &e) {
				printf("Error exception:%s\n", e.what());
			}
		}
	}

private:
// This is just a sample implementation of entity handler.
// Entity handler can be implemented in several ways by the manufacturer
	OCEntityHandlerResult entityHandler(
			std::shared_ptr<OCResourceRequest> request) {
		cout << "\tIn Server CPP entity handler:\n";
		OCEntityHandlerResult ehResult = OC_EH_ERROR;
		if (request) {
			// Get the request type and request flag
			std::string requestType = request->getRequestType();
			int requestFlag = request->getRequestHandlerFlag();

			if (requestFlag & RequestHandlerFlag::RequestFlag) {
				cout << "\t\trequestFlag : Request\n";
				auto pResponse = std::make_shared<OC::OCResourceResponse>();
				pResponse->setRequestHandle(request->getRequestHandle());
				pResponse->setResourceHandle(request->getResourceHandle());

				// Check for query params (if any)
				QueryParamsMap queries = request->getQueryParameters();

				if (!queries.empty()) {
					std::cout << "\nQuery processing upto entityHandler"
							<< std::endl;
				}
				for (auto it : queries) {
					std::cout << "Query key: " << it.first << " value : "
							<< it.second << std::endl;
				}

				// If the request type is GET
				if (requestType == "GET") {
					cout << "\t\t\trequestType : GET\n";

					pResponse->setErrorCode(200);
					pResponse->setResponseResult(OC_EH_OK);
					pResponse->setResourceRepresentation(get());
					if (firstaccess)
						firstaccess = false;
					if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
						ehResult = OC_EH_OK;
					}

				} else if (requestType == "PUT") {
					cout << "\t\t\trequestType : PUT\n";
					OCRepresentation rep = request->getResourceRepresentation();

					// Do related operations related to PUT request
					// Update the lightResource
					put(rep);
					pResponse->setErrorCode(200);
					pResponse->setResponseResult(OC_EH_OK);
					pResponse->setResourceRepresentation(get());
					if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
						ehResult = OC_EH_OK;
					}
				} else if (requestType == "POST") {
					cout << "\t\t\trequestType : POST\n";

					OCRepresentation rep = request->getResourceRepresentation();

					// Do related operations related to POST request

					pResponse->setErrorCode(200);
					pResponse->setResponseResult(OC_EH_OK);
					post(rep);
					if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
						ehResult = OC_EH_OK;
					}
				} else if (requestType == "DELETE") {
					cout << "Delete HueGroup" << endl;
					pResponse->setErrorCode(200);
					pResponse->setResponseResult(OC_EH_OK);
					deleteGroup();
					if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
						ehResult = OC_EH_OK;
					}
				}
			}

			if (requestFlag & RequestHandlerFlag::ObserverFlag) {
				ObservationInfo observationInfo = request->getObservationInfo();
				if (ObserveAction::ObserveRegister == observationInfo.action) {
					m_interestedObservers.push_back(observationInfo.obsId);
				} else if (ObserveAction::ObserveUnregister
						== observationInfo.action) {
					m_interestedObservers.erase(
							std::remove(m_interestedObservers.begin(),
									m_interestedObservers.end(),
									observationInfo.obsId),
							m_interestedObservers.end());
				}

				pthread_t threadId;

				cout << "\t\trequestFlag : Observer\n";
				gObservation = 1;
				static int startedThread = 0;

				// Observation happens on a different thread in ChangeLightRepresentation function.
				// If we have not created the thread already, we will create one here.
				if (!startedThread) {

					startedThread = 1;
				}
				ehResult = OC_EH_OK;
			}
		} else {
			std::cout << "Request invalid" << std::endl;
		}

		return ehResult;
	}

};

void PrintUsage() {
	std::cout << std::endl;
	std::cout << "Usage : hueserver <value>\n";
}

int main(int argc, char* argv[]) {
	PrintUsage();

	if (argc == 1) {
		isListOfObservers = false;
		isSecure = false;
	} else if (argc == 2) {
		int value = atoi(argv[1]);
		switch (value) {
		case 1:
			isListOfObservers = true;
			isSecure = false;
			break;
		case 2:
			isListOfObservers = false;
			isSecure = true;
			break;
		case 3:
			isListOfObservers = true;
			isSecure = true;
			break;
		case 4:
			isSlowResponse = true;
			break;
		default:
			break;
		}
	} else {
		return -1;
	}

	// Create PlatformConfig object
	PlatformConfig cfg { OC::ServiceType::InProc, OC::ModeType::Server,
			"0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
			0,         // Uses randomly available port
			OC::QualityOfService::LowQos };

	OCPlatform::Configure(cfg);
	try {
		// Create the instance of the resource class
		// (in this case instance of class 'LightResource').
		HueResource myHue;

		// Invoke createResource function of class light.
		myHue.createResource();
		std::cout << "Created resource." << std::endl;

		std::cout << "Added Interface and Type" << std::endl;

		// A condition variable will free the mutex it is given, then do a non-
		// intensive block until 'notify' is called on it.  In this case, since we
		// don't ever call cv.notify, this should be a non-processor intensive version
		// of while(true);
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

