#include "Server.h"
#include "Service.h"

CServer::CServer(const string& csServicesXmlPath,const string& csLogFilePath) : CSocketConnection(csLogFilePath)
{
	m_pController = new CController(csServicesXmlPath);
}

CServer::~CServer(void)
{
	if(m_pController != NULL)
	{
		delete m_pController;
	}
}

bool CServer::Init(const string& csServicesXmlPath, const string& csConfigXmlPath)
{
	pLogger->SetLoggerType(csConfigXmlPath);
	bool bOk = true;
	m_addrAddres.sin_addr.s_addr = INADDR_ANY;
	m_addrAddres.sin_family = AF_INET;
	CXmlModel xmlModel(csConfigXmlPath);
	do
	{
		if(xmlModel.Create())
		{
			const CNodeElement* pRoot = xmlModel.GetRootNode();
			int iIndex = -1;
			pRoot->HasChildWithName(CXmlKeyWords::csConnection, iIndex);
			pRoot = pRoot->GetChild(iIndex);
			string sPort = pRoot->GetAttributeValue(CXmlKeyWords::csPort).c_str();
			if(sPort == CNodeElement::csNoSuchData)
			{
				bOk = false;
				break;
			}
			m_addrAddres.sin_port = htons(atoi(sPort.c_str()));
		}
		else
		{
			bOk = false;
			break;
		}
		bOk = CSocketConnection::Init() && CreateSocket(m_scktListen) && Bind() && StartListening();
	}while(false);
	return bOk;
}

bool CServer::ReceiveRequest(int& argc, char**& argv)
{
	string sData;
	bool bOk = Receive(sData);
	if(bOk)
	{
		UnpackRequest(sData, argc, argv);
		string sMsg = std::to_string(argc);
		for(int i = 0; i < argc; ++i)
		{
			string s = argv[i];
			sMsg += ' ' + s;
		}
		CServer::GetLogger()->WriteProductionLog("Request received: " + sMsg);
	}
	return bOk;
}


bool CServer::SendResponce(const string& sData)
{
	return Send(sData);
}


void CServer::Launch()
{
	int argc;
	char** argv;
	string sMessage;
	while(true)
	{
		Accept();
		string sMsg ="Accepted connection from " + string(inet_ntoa(m_addrClientAddres.sin_addr));
		CServer::GetLogger()->WriteProductionLog(sMsg);
		if(ReceiveRequest(argc, argv))
		{
			string sData;
			CTable* Table = m_pController->Execute(argc, argv, sMessage);
			if(Table != NULL)
			{
				sData = CTable::ToXmlString(*Table);
				delete Table;
			}
			else
			{
				sData = CSocketConnection::csText + sMessage;
			}
			SendResponce(sData);		
		}
		ClearMemory(argc, argv);
	}
}


