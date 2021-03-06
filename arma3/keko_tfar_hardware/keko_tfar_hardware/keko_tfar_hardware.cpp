// keko_tfar_hardware.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

// code taken from / inspired by:
// https://playground.arduino.cc/Interfacing/CPPWindows
// http://killzonekid.com/arma-scripting-tutorials-how-to-make-arma-extension-part-1/
// https://community.bistudio.com/wiki/callExtension
// http://killzonekid.com/arma-scripting-tutorials-how-to-make-arma-extension-part-4/

#include <string>
#include <vector>
#include <iterator>
#include <sstream>

#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>

#include "SerialClass.h"

#define CURRENT_VERSION "1.0"

using namespace std;

struct Data
{
	bool ready = false;
	string params = "";
	string result = "";
};

unordered_map<long int, Data> tickets;
mutex mtx;

atomic<bool> worker_working(false);
long int id = 0; // global ticket id
long int cur_id = 0; // current ticket id

string readFromSerial();

Serial* SP;

extern "C"
{
	//--- Engine called on extension load 
	__declspec (dllexport) void __stdcall RVExtensionVersion(char *output, int outputSize);
	//--- STRING callExtension STRING
	__declspec (dllexport) void __stdcall RVExtension(char *output, int outputSize, const char *function);
	//--- STRING callExtension ARRAY
	__declspec (dllexport) int __stdcall RVExtensionArgs(char *output, int outputSize, const char *function, const char **args, int argsCnt);
}

string connectToSerial() {
	if (!SP) 
	{
		SP = new Serial("\\\\.\\COM8");
		if (SP->IsConnected())
			return "CONNECT SUCCESS";
		else
			return "CONNECT ERROR";
	}
	else
	{
		if (SP->IsConnected())
			return "ALREADY CONNECTED";
		else
			return "CONNECT ERROR";
	}	
}

string readFromSerial()
{
	char incomingData[256] = "";
	int dataLength = 255;
	int readResult = 0;

	// send some message to trigger output
	char out[1] = { 42 };
	SP->WriteData(out, 1);

	stringstream ss;
	ss.str("");
	while (SP->IsConnected())
	{
		readResult = SP->ReadData(incomingData, dataLength);
		if (readResult == 0)
			continue;

		incomingData[readResult] = 0;

		for (int i = 0; i < readResult; i++)
		{
			if (incomingData[i] == '#')
				return ss.str();
			else
				ss << incomingData[i];
		}
	}

	return ss.str();
}

void worker()
{
	while (worker_working = id > cur_id) // next ticket exists?
	{
		mtx.lock();
		Data ticket = tickets[++cur_id]; // copy ticket
		mtx.unlock();

		string input = ticket.params; // get input

		string output;
		if ((input.compare("disconnect")) == 0)
		{
			if (SP) 
			{
				SP->~Serial();
				if (SP->IsConnected()) 
					output = "DISCONNECT ERROR";				
				else				
					output = "DISCONNECT SUCCESS";				
				SP = nullptr;
			}
			else
			{
				output = "DISCONNECT NULLPTR";
			}
		}
		else if ((input.compare("connect")) == 0)
		{
			output = connectToSerial();
		}
		else if ((input.compare("get")) == 0)
		{
			connectToSerial();
			output = readFromSerial();			
		}
		else
		{
			output = "UNKKNOWN OPERATION";
		}
				
		ticket.result = output; // prepare result
		ticket.ready = true; // notify about result

		mtx.lock();
		tickets[cur_id] = ticket; // copy back the result
		mtx.unlock();
	}
}

//--- Extension version information shown in .rpt file
void __stdcall RVExtensionVersion(char *output, int outputSize)
{
	//--- max outputSize is 32 bytes
	strncpy_s(output, outputSize, CURRENT_VERSION, _TRUNCATE);
}

//--- name callExtension function
void __stdcall RVExtension(char *output, int outputSize, const char *function)
{
	if (!strncmp(function, "r:", 2)) // detect checking for result
	{
		long int num = atol(&function[2]); // ticket number or 0

		if (tickets.find(num) != tickets.end()) // ticket exists
		{
			mtx.lock();
			if (tickets[num].ready) // result is ready
			{
				strncpy_s(output, outputSize, tickets[num].result.c_str(), _TRUNCATE); // result
				tickets.erase(num); // get rid of the read ticket
				mtx.unlock();
				return;
			}
			mtx.unlock();

			strncpy_s(output, outputSize, "WAIT", _TRUNCATE); // result is not ready
			return;
		}
		strncpy_s(output, outputSize, "EMPTY", _TRUNCATE); // no such ticket
	}

	else if (!strncmp(function, "s:", 2)) // detect ticket submission
	{
		Data data;
		data.params = string(&function[2]); // extract params

		mtx.lock();
		tickets.insert(pair<long int, Data>(++id, data)); // add ticket to the queue
		mtx.unlock();

		if (!worker_working) // if worker thread is finished, start another one
		{
			worker_working = true;
			thread worker(worker);
			worker.detach(); // start parallel process
		}
		strncpy_s(output, outputSize, to_string(id).c_str(), _TRUNCATE); // ticket number
	}

	else
	{
		std::string str = function;
		strncpy_s(output, outputSize, ("input was: " + str).c_str(), _TRUNCATE);
	}	
}

//--- name callExtension [function, args]
int __stdcall RVExtensionArgs(char *output, int outputSize, const char *function, const char **args, int argsCnt)
{
	if (strcmp(function, "version") == 0)
	{
		//--- Parse args into vector
		vector<string> vec(args, next(args, argsCnt));

		ostringstream oss;		
		oss << CURRENT_VERSION;

		//--- Extension result
		strncpy_s(output, outputSize, (oss.str()).c_str(), _TRUNCATE);

		//--- Extension return code
		return 200;
	}
	
	else
	{
		strncpy_s(output, outputSize, "keko_tfar_hardware: available functions: version, settings", outputSize - 1);
		return -1;
	}
}