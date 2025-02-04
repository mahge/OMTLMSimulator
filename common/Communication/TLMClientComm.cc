/**
* File: TLMClientComm.cc
* 
* Implementation for the methods defined in TLMClientComm.h
*/
#include "Communication/TLMCommUtil.h"
#include "Communication/TLMClientComm.h"
#include "Logging/TLMErrorLog.h"
#include "Interfaces/TLMInterface.h"
#include <vector>
#include <deque>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
using std::ofstream;
using std::endl;

using std::vector;
using std::deque;
using std::string;

#ifndef WIN32
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#ifndef NOMINMAX
  #define NOMINMAX
#endif
#include <windows.h>
#include <cassert>
#include <io.h>
#endif

#ifndef _MSC_VER
#include <unistd.h> 
#endif

// Constructor
TLMClientComm::TLMClientComm()
    : SocketHandle(-1) {}

TLMClientComm::~TLMClientComm() {
    if(SocketHandle != -1) {
        close(SocketHandle);
    }
}


// Fill in TLMMessage with the information from TLMTimeData vector
// coming to given InterfaceID. This function is called by TLMPlugin
//  when constructing messages with time-stamped data.
void TLMClientComm::PackTimeDataMessageSignal(int InterfaceID,
                                              std::vector<TLMTimeDataSignal> &Data,
                                              TLMMessage &out_mess) {
    out_mess.Header.MessageType =  TLMMessageTypeConst::TLM_TIME_DATA;
    out_mess.Header.TLMInterfaceID = InterfaceID;
    out_mess.Header.SourceIsBigEndianSystem = TLMMessageHeader::IsBigEndianSystem;
    out_mess.Header.DataSize = Data.size() * sizeof(TLMTimeDataSignal);
    out_mess.Data.clear();
    out_mess.Data.resize(out_mess.Header.DataSize);
    memcpy(& out_mess.Data[0], & Data[0], out_mess.Header.DataSize);
}


// Fill in TLMMessage with the information from TLMTimeData vector 
// coming to given InterfaceID. This function is called by TLMPlugin
//  when constructing messages with time-stamped data.
void TLMClientComm::PackTimeDataMessage3D(int InterfaceID,
                                          std::vector<TLMTimeData3D> &Data,
                                          TLMMessage& out_mess) {
    out_mess.Header.MessageType =  TLMMessageTypeConst::TLM_TIME_DATA;
    out_mess.Header.TLMInterfaceID = InterfaceID;
    out_mess.Header.SourceIsBigEndianSystem = TLMMessageHeader::IsBigEndianSystem;
    out_mess.Header.DataSize = Data.size() * sizeof(TLMTimeData3D);
    out_mess.Data.clear();
    out_mess.Data.resize(out_mess.Header.DataSize);
    memcpy(& out_mess.Data[0], & Data[0], out_mess.Header.DataSize);
}

// Fill in TLMMessage with the information from TLMTimeData vector
// coming to given InterfaceID. This function is called by TLMPlugin
//  when constructing messages with time-stamped data.
void TLMClientComm::PackTimeDataMessage1D(int InterfaceID,
                                          std::vector<TLMTimeData1D> &Data,
                                          TLMMessage& out_mess) {
    out_mess.Header.MessageType =  TLMMessageTypeConst::TLM_TIME_DATA;
    out_mess.Header.TLMInterfaceID = InterfaceID;
    out_mess.Header.SourceIsBigEndianSystem = TLMMessageHeader::IsBigEndianSystem;
    out_mess.Header.DataSize = Data.size() * sizeof(TLMTimeData1D);
    out_mess.Data.clear();
    out_mess.Data.resize(out_mess.Header.DataSize);
    memcpy(& out_mess.Data[0], & Data[0], out_mess.Header.DataSize);
}


// Unpack TLMTimeData from TLMMessage3D into Data queue
void TLMClientComm::UnpackTimeDataMessageSignal(TLMMessage &mess, std::deque<TLMTimeDataSignal> &Data) {

    // since mess.Data is continious we can just convert the pointer
    TLMTimeDataSignal* Next = (TLMTimeDataSignal*)(&mess.Data[0]);

    // check if we have byte order missmatch in the message and perform
    // swapping if necessary
    bool switch_byte_order =
        (TLMMessageHeader::IsBigEndianSystem != mess.Header.SourceIsBigEndianSystem);
    if(switch_byte_order)
        TLMCommUtil::ByteSwap(Next, sizeof(double),  mess.Header.DataSize/sizeof(double));

    for(unsigned i = 0; i < mess.Header.DataSize/sizeof(TLMTimeDataSignal); i++, Next++) {
        if(TLMErrorLog::GetLogLevel() >= TLMLogLevel::Info) {
           TLMErrorLog::Info(" RECV for time= " + TLMErrorLog::ToStdStr(Next->time));
        }
        Data.push_back(*Next);
    }
}

// Unpack TLMTimeData from TLMMessage3D into Data queue
void TLMClientComm::UnpackTimeDataMessage3D(TLMMessage& mess, deque<TLMTimeData3D>& Data) {

    // since mess.Data is continious we can just convert the pointer
    TLMTimeData3D* Next = (TLMTimeData3D*)(&mess.Data[0]);

    // check if we have byte order missmatch in the message and perform
    // swapping if necessary
    bool switch_byte_order = 
        (TLMMessageHeader::IsBigEndianSystem != mess.Header.SourceIsBigEndianSystem);
    if(switch_byte_order)
        TLMCommUtil::ByteSwap(Next, sizeof(double),  mess.Header.DataSize/sizeof(double));

    for(unsigned i = 0; i < mess.Header.DataSize/sizeof(TLMTimeData3D); i++, Next++) {
        if(TLMErrorLog::GetLogLevel() >= TLMLogLevel::Info) {
            TLMErrorLog::Info(" RECV for time= " + TLMErrorLog::ToStdStr(Next->time));
        }
        Data.push_back(*Next);
    }
}

// Unpack TLMTimeData from TLMMessage1D into Data queue
void TLMClientComm::UnpackTimeDataMessage1D(TLMMessage& mess, deque<TLMTimeData1D>& Data) {

    // since mess.Data is continious we can just convert the pointer
    TLMTimeData1D* Next = (TLMTimeData1D*)(&mess.Data[0]);

    // check if we have byte order missmatch in the message and perform
    // swapping if necessary
    bool switch_byte_order =
        (TLMMessageHeader::IsBigEndianSystem != mess.Header.SourceIsBigEndianSystem);
    if(switch_byte_order)
        TLMCommUtil::ByteSwap(Next, sizeof(double),  mess.Header.DataSize/sizeof(double));

    for(unsigned i = 0; i < mess.Header.DataSize/sizeof(TLMTimeData1D); i++, Next++) {
        if(TLMErrorLog::GetLogLevel() >= TLMLogLevel::Info) {
            TLMErrorLog::Info(" RECV for time= " + TLMErrorLog::ToStdStr(Next->time));
        }
        Data.push_back(*Next);
    }
}

// ConnectManager function tries to establish a TCP/IP connection
// to the TLM manager.Returns socket handle on success.
// Input: hostname (callname) & port number
int TLMClientComm::ConnectManager(string& callname, int portnr) {
    struct sockaddr_in sa;
    int s,count;

#ifdef WIN32
    DWORD e;
    WSADATA ws; 
    e=WSAStartup(0x0101,&ws);
    assert(e==0);
    TLMErrorLog::Info("WinSocket started\n");

    sa.sin_family=AF_INET;

    struct hostent *hp;
    char* localIP;

    hp = gethostbyname(callname.c_str());
    if(hp == NULL) {
        TLMErrorLog::FatalError(string("TLM: Cannot resolve the host ") + callname);
        return(-1);
    }
    localIP = inet_ntoa (*(struct in_addr *)*hp->h_addr_list);

    //    sa.sin_addr =*((struct in_addr *)(hp->h_addr));
    //     if(0 == strcmp(callname.c_str(), "localhost")) {
    sa.sin_addr.s_addr = inet_addr(localIP);

    sa.sin_port=htons((u_short)portnr);

    s = socket(hp->h_addrtype, SOCK_STREAM, IPPROTO_TCP);

#else
    TLMErrorLog::Info("Trying to find TLM manager host " + callname);


    memset(&sa, 0 , sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons((u_short)portnr);
    s = socket(AF_INET,SOCK_STREAM,0);
#endif

    if(s < 0) {
        TLMErrorLog::FatalError("TLM: Can not contact TLM manager");
    }
    else {
        TLMErrorLog::Info("TLM manager host found, trying to connect...");
    }

#ifdef WIN32
    const char val = 1;
#else
    int val = 1;
#endif
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    count = 0;

    while(connect(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        count++;
        TLMErrorLog::Info(string("Connection attempt ") +  TLMErrorLog::ToStdStr(count) + " failed");
        if(count>=10) {
#ifndef WIN32
            close(s);
#else
            closesocket(s); // BZ306, do not use close() on Windows.
            WSACleanup();
#endif
            TLMErrorLog::FatalError("TLM: Can not connect to manager");
        }

        TLMErrorLog::Info("Pausing...");
#ifndef WIN32
        usleep(count * count * 1000000); // micro seconds
#else
        Sleep(count * count * 1000); // milli seconds
#endif        
        TLMErrorLog::Info("Trying again...");
    }


    SocketHandle = s;

    return(s);
}

void TLMClientComm::CreateComponentRegMessage(std::string& Name, TLMMessage& mess) {
    mess.Header.MessageType = TLMMessageTypeConst::TLM_REG_COMPONENT;
    mess.Header.DataSize = Name.length();
    mess.Data.resize(Name.length());
    memcpy(&mess.Data[0], Name.c_str(), Name.length());
}

void TLMClientComm::CreateInterfaceRegMessage(std::string& Name, int dimensions,
                                              std::string &causality, std::string domain, TLMMessage& mess) {
    mess.Header.MessageType = TLMMessageTypeConst::TLM_REG_INTERFACE;
#ifdef INTERFACE_TYPES
    std::stringstream ss;
    ss << Name << ":" << dimensions << ":" << causality << ":" << domain;
    std::string specification = ss.str();
    TLMErrorLog::Info("Client sends nameAndType: "+specification);
#else
    std::string specification = Name;
    TLMErrorLog::Info("Client sends name: "+specification);
#endif

    mess.Header.DataSize = specification.length();
    mess.Data.resize(specification.length());
    memcpy(&mess.Data[0], specification.c_str(), specification.length());
}

void TLMClientComm::CreateParameterRegMessage(std::string &Name, std::string &Value, TLMMessage &mess) {
    mess.Header.MessageType = TLMMessageTypeConst::TLM_REG_PARAMETER;

    std::string nameAndValue = Name+":"+Value;
    TLMErrorLog::Info("Client sends nameAndValue: "+nameAndValue);
    mess.Header.DataSize = nameAndValue.length();
    mess.Data.resize(nameAndValue.length());
    memcpy(&mess.Data[0], nameAndValue.c_str(), nameAndValue.length());
}

void TLMClientComm::UnpackRegInterfaceMessage(TLMMessage& mess, TLMConnectionParams& param) {
    if(mess.Header.DataSize == 0) return; // non connected interface
    if(mess.Header.DataSize != sizeof(TLMConnectionParams)) {
        TLMErrorLog::FatalError("Wrong size of message in interface registration : DataSize "+
            std::to_string(mess.Header.DataSize)+
            " sizeof(TLMConnectionParams)="+
            std::to_string(sizeof(TLMConnectionParams)));
    }

    // check if we have byte order missmatch in the message and perform
    // swapping if necessary
    bool switch_byte_order = 
        (TLMMessageHeader::IsBigEndianSystem != mess.Header.SourceIsBigEndianSystem);
    if(switch_byte_order)
        TLMCommUtil::ByteSwap(& mess.Data[0], sizeof(double),  
        mess.Header.DataSize/sizeof(double)); 

    memcpy(&param, & mess.Data[0], mess.Header.DataSize);
}

void TLMClientComm::UnpackRegParameterMessage(TLMMessage &mess, std::string &Value) {
    TLMErrorLog::Info("Entering UnpackRegParameterMessage()");
    if(mess.Header.DataSize == 0) return; // non connected interface
    TLMErrorLog::Info("DataSize is ok!");
    char ValueBuf[100];
    if(mess.Header.DataSize != sizeof(ValueBuf)) {
        TLMErrorLog::FatalError("Wrong size of message in parameter registration : DataSize "+
            std::to_string(mess.Header.DataSize)+
            " sizeof(ValueBuf)="+
            std::to_string(sizeof(ValueBuf)));
    }

    // check if we have byte order missmatch in the message and perform
    // swapping if necessary
    bool switch_byte_order =
        (TLMMessageHeader::IsBigEndianSystem != mess.Header.SourceIsBigEndianSystem);
    if(switch_byte_order)
        TLMCommUtil::ByteSwap(& mess.Data[0], sizeof(double),
        mess.Header.DataSize/sizeof(double));

    memcpy(&ValueBuf, & mess.Data[0], mess.Header.DataSize);
    Value = std::string(ValueBuf);

    TLMErrorLog::Info("Parameter received value: "+Value);
}
