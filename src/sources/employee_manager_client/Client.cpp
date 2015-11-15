#include "Client.h"
#include <iostream>
using namespace std;

vector<string> CClient::vsHelpInfo;

CClient::~CClient(void)
{
}

bool CClient::Init(const string&  csConfigXmlPath)
{
	CSocketConnection::GetLogger()->SetLoggerType(csConfigXmlPath);
	bool bOk = true;
	InitHelp();
	m_addrAddres.sin_family = AF_INET;
	CXmlModel XmlModel(csConfigXmlPath);
	do
	{
		if(XmlModel.Create())
		{
			const CNodeElement* pRoot = XmlModel.GetRootNode();
			int iIndex = -1;
			bOk = pRoot->HasChildWithName(CXmlKeyWords::csConnection, iIndex);
			if(!bOk)
			{
				string sMsg ="No connection tag found in client.xml";
				CSocketConnection::GetLogger()->WriteDebugLog(sMsg, true);
				break;
			}
			pRoot = pRoot->GetChild(iIndex);
			string sIp = pRoot->GetAttributeValue(CXmlKeyWords::csIP).c_str();
			if(sIp == CNodeElement::csNoSuchData)
			{
				string sMsg ="No IP attribute in " + csConfigXmlPath + " found!";
				CSocketConnection::GetLogger()->WriteDebugLog(sMsg, true);
				bOk = false;
				break;
			}
			m_addrAddres.sin_addr.s_addr = inet_addr(sIp.c_str());
			string sPort = pRoot->GetAttributeValue(CXmlKeyWords::csPort).c_str();
			if(sPort == CNodeElement::csNoSuchData)
			{
				string sMsg ="No port attribute in " + csConfigXmlPath + " found!";
				CSocketConnection::GetLogger()->WriteDebugLog(sMsg, true);
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
	bOk = CSocketConnection::Init() && CreateSocket(m_scktConnect);
	}while(false);
	return bOk;
}

bool CClient::SendRequest(int argc, char** argv)
{
	return Connect() && Send(PackRequest(argc, argv));
}

bool CClient::ReceiveResponse(string& sData)
{
	return Receive(sData);
}

void CClient::Launch(int argc, char**& argv)
{
	string sCommand;
	string sParams;
	do
	{
		if(argc > 1)
		{
			sCommand = argv[1];
		}
		if(argc > 2)
		{
			sParams = argv[2];
		}
		if(!CheckCommand(sCommand, sParams))
		{
			break;
		}

		if(sCommand.empty() || sCommand == CCommands::csCmdHelp)
		{
			ShowHelp();
			break;
		}

		if(!SendRequest(argc, argv))
		{
			FreeWinSock();
		}
		string sReceivedData;
		if(!ReceiveResponse(sReceivedData))
		{
			FreeWinSock();
		}
		else
		{
			Shutdown();
			if(sReceivedData.substr(0,3) == CSocketConnection::csText)
			{
				printf("%s\n", sReceivedData.substr(3, sReceivedData.size()).c_str());
			}
			else
			{
				CTable Table = CTable::CreateFromXmlString(sReceivedData);
				Table.Print();
			}
		}
	}while(false);
		FreeWinSock();
}

void CClient::InitHelp()
{
	if(vsHelpInfo.empty())
	{
		vsHelpInfo.push_back("employee_manager.exe get_employees --options=\"employees_id=1[2,3]&show_relations=(0|1)\"");
		vsHelpInfo.push_back("employee_manager.exe get_employees_data --options=employees_id=1[2,3]");
		vsHelpInfo.push_back("employee_manager.exe get_employees_relations --options=\"boss_id=1&inferior_id=2\"");
		vsHelpInfo.push_back("employee_manager.exe get_inferiors --boss_id=1");
		vsHelpInfo.push_back("employee_manager.exe get_boss --inferior_id=1");
		vsHelpInfo.push_back("employee_manager.exe get_colleagues --employee_id=1");
		vsHelpInfo.push_back("employee_manager.exe help");
		vsHelpInfo.push_back("employee_manager.exe validate_configuration");
	}
}

string CClient::GetHelpInfoLine(size_t iLineIndex)
{
	assert(iLineIndex < 8);
	return vsHelpInfo[iLineIndex];
}

void CClient::ShowHelp()
{
	for(size_t i = 0; i < vsHelpInfo.size(); ++i)
	{
		printf("%s\n", vsHelpInfo[i].c_str());
	}
}

bool CClient::HasParams(const string& csData, const string& sParam1, bool bStrict)
{
	if(csData.empty())
	{
		if(!bStrict)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool bOk = true;
	if(bStrict)
	{
		bOk = StartsWith(csData, CCommands::csMinusMinus) && (csData.find(sParam1 + '=') != string::npos);
	}
	else
	{
		bOk = StartsWith(csData, CCommands::csMinusMinus + CCommands::csOptions) && (csData.find(sParam1 + '=') != string::npos);
	}
	return bOk;
}

bool CClient::HasParams(const string& csData, const string& sParam1, const string& sParam2)
{
	if(csData.empty())
	{
		return true;
	}
	if(csData.find("&") != string::npos)
	{
		return (HasParams(csData, sParam1) && HasParams(csData, sParam2));
	}
	else
	{
		return (HasParams(csData, sParam1) ^ HasParams(csData, sParam2));
	}
}

bool CClient::StartsWith(const string& csData, const string& sParam)
{
	size_t iParamSize = sParam.size();
	if(csData.substr(0, iParamSize) == sParam)
	{
		return true;
	}
	return false;

}

bool CClient::CheckCommand(const string& csData, const string& sParams)
{
	bool bOk = false;
	int iHelpIndex = -1;
	do
	{
		if(csData.empty())
		{
			bOk = true;
			break;
		}
		if(csData == CCommands::csCmdGetEmployees)
		{
			bOk = HasParams(sParams, CCommands::csEmployeesId, CCommands::csShowRelations);
			if(!bOk)
			{
				iHelpIndex = 0;
			}
			break;
		}
		if(csData == CCommands::csCmdGetEmployeesData)
		{
			bOk = HasParams(sParams, CCommands::csEmployeesId);
			if(!bOk)
			{
				iHelpIndex = 1;
			}
			break;
		}
		if(csData == CCommands::csCmdGetEmployeesRelations)
		{
			bOk = HasParams(sParams, CCommands::csBossId, CCommands::csInferiorId);
			if(!bOk)
			{
				iHelpIndex = 2;
			}
			break;
		}
		if(csData == CCommands::csCmdGetInferiors)
		{
			bOk = HasParams(sParams, CCommands::csBossId, true);
			if(!bOk)
			{
				iHelpIndex = 3;
			}
			break;
		}
		if(csData == CCommands::csCmdGetBoss)
		{
			bOk = HasParams(sParams, CCommands::csInferiorId, true);
			if(!bOk)
			{
				iHelpIndex = 4;
			}
			break;
		}
		if(csData == CCommands::csCmdGetColleagues)
		{
			bOk = HasParams(sParams, CCommands::csEmployeeId, true);
			if(!bOk)
			{
				iHelpIndex = 5;
			}
			break;
		}
		if(csData == CCommands::csCmdValidateConfiguration)
		{
			bOk = true;
			break;
		}
		if(csData == CCommands::csCmdHelp)
		{
			bOk = true;
			break;
		}
	}while(false);

	if(!bOk)
	{
		printf("Bad command!\n");
	}
	if(iHelpIndex != -1)
	{
		printf("%s\n", GetHelpInfoLine(iHelpIndex).c_str());
	}
	else
		if(!bOk)
		{
				ShowHelp();
		}
	return bOk;
}