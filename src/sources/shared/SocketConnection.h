#pragma once
#include "winsock2.h"
#include "../shared/XmlModel.h"
#include "../shared/XmlKeyWords.h"
#include "../shared/Logger.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

class CSocketConnection
{
public:
	CSocketConnection(const string& csLogFilePath);
	~CSocketConnection(void);
	bool Init();
	bool Send(const string& csData);
	bool Receive(string& sData);
	static string PackRequest(int argc, char** argv);
	static void UnpackRequest(const string& csXmlRequest, int& argc, char**& argv);
	static CLogger* GetLogger() { return pLogger; }

private:

protected:
	static CLogger* pLogger;
	int m_iConnectionPort;
	WSADATA m_wsaData;
	WORD m_wrdVersion;
	SOCKET m_scktConnect;
	SOCKADDR_IN m_addrAddres;
	SOCKET m_scktListen;
	SOCKADDR_IN m_addrClientAddres;
	string m_sLogFilePath;

protected:
	void CSocketConnection::ErrorCloseConnecton(const string& sInfo);
	bool LoadWinSock();
	void FreeWinSock();
	bool CreateSocket(SOCKET& Socket);
	bool Bind();
	bool StartListening();
	bool Connect();
	void Shutdown();
	void Accept();
	static void ClearMemory(int argc, char** argv);

public:
	static const string csText;

};

