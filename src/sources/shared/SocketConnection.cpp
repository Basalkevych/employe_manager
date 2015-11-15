#include "SocketConnection.h"

CLogger* CSocketConnection::pLogger = NULL;

const string CSocketConnection::csText = "{T}";

CSocketConnection::CSocketConnection(const string& csLogFilePath) : m_wrdVersion(MAKEWORD(2, 1)), m_sLogFilePath(csLogFilePath)
{
	if(pLogger == NULL)
	{
		pLogger = new CLogger(m_sLogFilePath);
	}
}

CSocketConnection::~CSocketConnection(void)
{
	if(pLogger != NULL)
	{
		delete pLogger;
	}
}

void CSocketConnection::ErrorCloseConnecton(const string& sInfo)
{
	pLogger->WriteProductionLog(sInfo, true);
	shutdown(m_scktConnect, 2);
}

bool CSocketConnection::LoadWinSock() 
{
	bool bOk = true;
	if(WSAStartup(m_wrdVersion, &m_wsaData) == SOCKET_ERROR)
	{
		pLogger->WriteDebugLog("winsock2 library init error");
		bOk = false;
	}
	return bOk;
}

void CSocketConnection::FreeWinSock()
{
	WSACleanup();
}

void CSocketConnection::Shutdown()
{
	shutdown(m_scktConnect, 2);
}

bool CSocketConnection::CreateSocket(SOCKET& Socket)
{
	bool bOk = true;
	if((Socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		pLogger->WriteDebugLog("Error while creating socket");
		bOk = true;
	}
	return bOk;
}

bool CSocketConnection::Bind()
{
	bool bOk = true;
	if(bind(m_scktListen, (LPSOCKADDR)&m_addrAddres, sizeof(m_addrAddres)) == SOCKET_ERROR)
	{
		pLogger->WriteDebugLog("Error while binding");
		
		bOk = false;
	}
	return bOk;
}

bool CSocketConnection::StartListening()
{
	bool bOk = true;
	if(listen(m_scktListen, SOMAXCONN) == SOCKET_ERROR)
	{
		pLogger->WriteDebugLog("Error while listening");
		bOk = false;
	}
	return bOk;
}

bool CSocketConnection::Connect()
{
	bool bOk = true;
	if(connect(m_scktConnect, (SOCKADDR*)&m_addrAddres, sizeof(m_addrAddres)) == SOCKET_ERROR)
	{
		pLogger->WriteDebugLog("Connection error");
		bOk = false;
	}
	return bOk;
}

bool CSocketConnection::Init()
{
	return LoadWinSock();
}

bool CSocketConnection::Send(const string& csData)
{
	bool bOk = true;
	const string csErrSendMsg = "Error while sending data!";
	int iDataSize = (csData.size() + 1) * sizeof(csData[0]);
	void* p = &iDataSize;
	do
	{
		if(send(m_scktConnect, static_cast<char*>(p), sizeof(iDataSize), MSG_DONTROUTE) == SOCKET_ERROR)
		{
			ErrorCloseConnecton(csErrSendMsg);
			bOk = false;
			break;
		}
		if(send(m_scktConnect, csData.c_str(), iDataSize, MSG_DONTROUTE) == SOCKET_ERROR)
		{
			ErrorCloseConnecton(csErrSendMsg);
			return bOk = false;
			break;
		}
	}while(false);

	return bOk;
}

bool CSocketConnection::Receive(string& sData)
{
	sData.clear();
	bool bOk = true;
	const string csErrSendMsg = "Error while receiving data!";
	char* DataSize = new char[sizeof(int)];
	do
	{
		if(recv(m_scktConnect,DataSize, sizeof(int), MSG_WAITALL) == SOCKET_ERROR)
		{
			ErrorCloseConnecton(csErrSendMsg);
			delete[] DataSize;
			bOk = false;
			break;
		}
		int iBytesReceived = (*(int*)DataSize);
		delete[] DataSize;
		#ifdef _DEBUG
			printf("%d\n", iBytesReceived);
		#endif

		char* buff = new char[iBytesReceived];
		if(recv(m_scktConnect,buff, iBytesReceived, MSG_WAITALL) == SOCKET_ERROR)
		{
			ErrorCloseConnecton(csErrSendMsg);
			delete[] buff;
			bOk = false;
			break;
		}
		sData = buff;
		delete[] buff;
	}while(false);

	return bOk;
}

string CSocketConnection::PackRequest(int argc, char** argv)
{
	string sMsg = "Sending request: " + std::to_string(argc);
	for(int i = 0; i < argc; ++i)
	{
		string s = argv[i];
		sMsg += ' ' + s;
	}
	pLogger->WriteProductionLog(sMsg);
	string sXmlRequest;
	sXmlRequest += '<' + CXmlKeyWords::csRequest + ' ' + CXmlKeyWords::csNArgc + "=\"" + std::to_string(argc) + "\">";
	for(int i = 0; i < argc; ++i)
	{
		sXmlRequest += '<' + CXmlKeyWords::csData + '>';
		sXmlRequest += argv[i];
		sXmlRequest += "</" + CXmlKeyWords::csData + '>';
	}
	sXmlRequest += "</" + CXmlKeyWords::csRequest + '>';
	return sXmlRequest;
}

void CSocketConnection::UnpackRequest(const string& csXmlRequest, int& argc, char**& argv)
{
	CXmlModel XmlModel(csXmlRequest, false);
	if(XmlModel.Create())
	{
		const CNodeElement* pRoot = XmlModel.GetRootNode();
		argc = atoi(pRoot->GetAttributeValue(CXmlKeyWords::csNArgc).c_str());
		argv = new char*[argc];
		for(size_t i = 0; i < pRoot->GetNumberOfChildren(); ++i)
		{
			string sParam = pRoot->GetChild(i)->GetText();
			size_t iSizeInBytes = (sParam.size() + 1) * sizeof(sParam[0]);
			argv[i] = new char[iSizeInBytes];
			strncpy_s(argv[i], iSizeInBytes, sParam.c_str(), sParam.size() + 1);
		}
	}

}

void CSocketConnection::Accept()
{
	int iClientAddresLength = sizeof(m_addrClientAddres);
	m_scktConnect = accept(m_scktListen, (SOCKADDR*)&m_addrClientAddres, &iClientAddresLength);
}

void CSocketConnection::ClearMemory(int argc, char** argv)
{
	if(argv != NULL)
	{
		for(int i = 0; i < argc; ++i)
		{
			if(argv[i] != NULL)
			{
				delete[] argv[i];
			}
		}
		delete[] argv;
	}
}