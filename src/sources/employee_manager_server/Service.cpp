#include "Service.h"
#include <iostream>
using namespace std;

SERVICE_STATUS CService::ServiceStatus;
SERVICE_STATUS_HANDLE CService::StatusHandle;
string CService::sServiceName;
string CService::sServiceShownName;
string CService::sServiceDescription;
SERVICE_TABLE_ENTRY CService::ServiceTable[1];
CServer* CService::pServer = NULL;
string CService::sServicePath;

const string CService::csInstallCmd = "install";
const string CService::csUninstallCmd = "uninstall";
const string CService::csStartCmd = "start";
const string CService::csStopCmd = "stop";
const string CService::csRestartCmd = "restart";

CService::CService(void)
{
}

CService::~CService(void)
{
}

string CService::GetServicePath()
{
	const size_t ciMaxSize = 500;
	if(!sServicePath.empty())
	{
		return sServicePath;
	}
	string sRez;
	char szPath[300];
	if(!GetModuleFileNameA(NULL, reinterpret_cast<LPSTR>(szPath), ciMaxSize))
    {
		CSocketConnection::GetLogger()->WriteDebugLog("GetModuleFileNameA function failed");
    }
	 else
	 {
		 sRez = static_cast<char*>(szPath);
		 size_t iLastSlashPos = sRez.rfind('\\');
		 sRez.erase(iLastSlashPos, sRez.size());
	 }
	 sServicePath = sRez;
	 return sRez;
}

CService::ServStatus CService::IsStopped(string& sMsg)
{
	ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus (StatusHandle, &ServiceStatus);
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hService = OpenService(hSCM, reinterpret_cast<LPCSTR>(CService::sServiceName.c_str()), SERVICE_STOP | SERVICE_QUERY_STATUS);
	SERVICE_STATUS_PROCESS ServiceStatusProcess;
	DWORD dwBytesNeeded;
	ServStatus Status = STOPPED;
	do
	{
		if(hSCM == NULL)
		{
			sMsg = "hSCM == NULL";
			Status = UNKNOWN;
			break;
		}
		if(hService == NULL)
		{
			sMsg = "hService == NULL";
			Status = UNKNOWN;
			break;
		}
	   // Make sure the service is not already stopped
		if(!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ServiceStatusProcess,
			sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
		{
			sMsg = "QueryServiceStatusEx function failed";
			Status = UNKNOWN;
			break;
		}

		if(ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED) 
		{
			Status = ALREADY_STOPPED;
			break;
		}

	   // If a stop is pending, just wait for it
		while(ServiceStatusProcess.dwCurrentState == SERVICE_STOP_PENDING ) 
		{
			sMsg = "Service stop pending...";
			if(!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ServiceStatusProcess, 
			   sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ))
			{
				sMsg +="\nQueryServiceStatusEx function failed";
				Status = UNKNOWN;
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCM);
				return Status;
			}

			if(ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
			{
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCM);
				return ALREADY_STOPPED;
			}
		}

		// Send a stop code to the main service
		if (!ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ServiceStatusProcess))
		{
			sMsg = "ControlService function failed";
			Status = UNKNOWN;
			break;
		}

		// Wait for the service to stop
		while (ServiceStatusProcess.dwCurrentState != SERVICE_STOPPED) 
		{
			if(!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ServiceStatusProcess, 
			   sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
			{
				sMsg = "QueryServiceStatusEx function failed";
				Status = UNKNOWN;
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCM);
				return Status;
			}

			if(ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
			{
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCM);
				return STOPPED;
			}
		}
	}while(false);
	CloseServiceHandle(hService); 
	CloseServiceHandle(hSCM);
	return Status;
}

void CService::Stop()
{
	string sMsg;
	ServStatus Status = IsStopped(sMsg);
	if(!sMsg.empty())
	{
		CSocketConnection::GetLogger()->WriteDebugLog(sMsg);
	}
	switch(Status)
	{
	case ALREADY_STOPPED:
		CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + " is already stopped.",true);
		break;
	case STOPPED:
		CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + " is successfully stopped.",true);
		break;
	case UNKNOWN:
		CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + " is no installed.",true);
		break;
	}
}


bool CService::Init()
{
	return pServer->Init(GetServicePath() + "\\config\\employees.xml", GetServicePath()  + "\\config\\server.xml");
}

void CService::Main(int argc, char** argv)
{
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;

	StatusHandle = RegisterServiceCtrlHandler(reinterpret_cast<LPCSTR>(const_cast<char*>(sServiceName.c_str())), reinterpret_cast<LPHANDLER_FUNCTION>(ControlHandler));
	if (StatusHandle == static_cast<SERVICE_STATUS_HANDLE>(0)) 
	{
		return;
	}

	if(!Init())
	{
		ServiceStatus.dwCurrentState    = SERVICE_STOPPED;
		ServiceStatus.dwWin32ExitCode   = -1;
		SetServiceStatus(StatusHandle, &ServiceStatus);
		if(pServer != NULL)
		{
			delete pServer;
		}
		return; 
	}

	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(StatusHandle, &ServiceStatus);

	if(ServiceStatus.dwCurrentState == SERVICE_RUNNING)
	{
		pServer->Launch();
	}
	delete pServer;
}

void CService::Install(const char* argv0)
{
	string sArgv0 = argv0;
	size_t iPos = sArgv0.rfind('\\');
	if(iPos != string::npos)
	{
		sArgv0.erase(0, iPos + 1);
	}
	string sFullPath = GetServicePath() + '\\' + sArgv0;
	
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if(!hSCManager) 
	{
		CSocketConnection::GetLogger()->WriteDebugLog("Can't open Service Control Manager");
		CSocketConnection::GetLogger()->WriteProductionLog("You don't have rights to install service.", true);
		return;
	}
  
	SC_HANDLE hService = CreateService(
		hSCManager,
		reinterpret_cast<LPCSTR>(sServiceName.c_str()),
		reinterpret_cast<LPCSTR>(sServiceShownName.c_str()),
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		reinterpret_cast<LPCSTR>(sFullPath.c_str()),
		NULL, NULL, NULL, NULL, NULL
	);

	SERVICE_DESCRIPTION description;
	description.lpDescription = reinterpret_cast<LPSTR>(const_cast<char*>(sServiceDescription.c_str()));
	ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &description);

	if(!hService) 
	{
		int err = GetLastError();
		switch(err) 
		{
		case ERROR_ACCESS_DENIED:
			CSocketConnection::GetLogger()->WriteProductionLog("Access to service " + sServiceName + " is denied.", true);
			break;
		case ERROR_CIRCULAR_DEPENDENCY:
			CSocketConnection::GetLogger()->WriteProductionLog("Circular dependensy.", true);
			break;
		case ERROR_DUPLICATE_SERVICE_NAME:
			CSocketConnection::GetLogger()->WriteProductionLog("Service name " + sServiceName + " duplicates.", true);
			break;
		case ERROR_INVALID_HANDLE:
			CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + "is already installed.", true);
			break;
		case ERROR_INVALID_NAME:
			CSocketConnection::GetLogger()->WriteProductionLog("Service name" + sServiceName + "is invalid.", true);
			break;
		case ERROR_INVALID_PARAMETER:
			CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + " has invalid parameter.", true);
			break;
		case ERROR_INVALID_SERVICE_ACCOUNT:
			CSocketConnection::GetLogger()->WriteProductionLog("Invalid service account.", true);
			break;
		case ERROR_SERVICE_EXISTS:
			CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + "already exists.", true);
			break;
		default:
			CSocketConnection::GetLogger()->WriteProductionLog("Error: undefined", true);
		}
		CloseServiceHandle(hSCManager);
		return;
	}
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	string sInfo = "Service " + sServiceName + " has been installed successfully.";
	CSocketConnection::GetLogger()->WriteProductionLog(sInfo, true);
}

void CService::Uninstall() 
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(!hSCManager)
	{
		CSocketConnection::GetLogger()->WriteDebugLog("Can't open Service Control Manager.");
		CSocketConnection::GetLogger()->WriteProductionLog("You don't have rights to uninstall service.", true);
		return;
	}
	SC_HANDLE hService = OpenService(hSCManager, reinterpret_cast<LPCSTR>(sServiceName.c_str()), SERVICE_STOP | DELETE);
	if(!hService)
	{
		CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + " does not exist.", true);
		CloseServiceHandle(hSCManager);
		return;
	}
	DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + " has been uninstalled successfully.", true);
}

void CService::Start() 
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	SC_HANDLE hService = OpenService(hSCManager, reinterpret_cast<LPCSTR>(sServiceName.c_str()), SERVICE_START);
	if(!StartService(hService, 0, NULL)) 
	{
		int err = GetLastError();
		if(err == ERROR_SERVICE_ALREADY_RUNNING)
		{
			CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + " is already started.", true);
		}
		else
		{
			CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + " is not installed.", true);
		}
		CloseServiceHandle(hSCManager);
		return ;
	}
	CSocketConnection::GetLogger()->WriteProductionLog("Service " + sServiceName + " has been started successfully.", true);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return;
}

void CService::ControlHandler(DWORD request)
{
	string sMsg;
	switch(request)
	{ 
		case SERVICE_CONTROL_STOP:
			IsStopped(sMsg);
			return;

		case SERVICE_CONTROL_SHUTDOWN:
			IsStopped(sMsg);
			return;
    
		default:
			break;
	}
	SetServiceStatus (StatusHandle, &ServiceStatus);
	return;
} 

bool CService::GetServiceParameters()
{
	CXmlModel xmlModel(GetServicePath() + "\\config\\server.xml");
	bool bOk = xmlModel.Create();
	if(!bOk)
	{
		return false;
	}
	const CNodeElement* pRoot = xmlModel.GetRootNode();
	int iIndex = -1;
	pRoot->HasChildWithName(CXmlKeyWords::csService, iIndex);
	pRoot = pRoot->GetChild(iIndex);
	for(size_t i = 0; i < pRoot->GetNumberOfChildren(); ++i)
	{
		if(pRoot->GetChild(i)->GetName() == CXmlKeyWords::csName)
		{
			sServiceName = pRoot->GetChild(i)->GetText();
		}
		else if(pRoot->GetChild(i)->GetName() == CXmlKeyWords::csShownName)
		{
			sServiceShownName = pRoot->GetChild(i)->GetText();
		}
		else if(pRoot->GetChild(i)->GetName() == CXmlKeyWords::csDescription)
		{
			sServiceDescription = pRoot->GetChild(i)->GetText();
		}
	}
	return true;
}

void CService::Launch(int argc, char** argv)
{
	pServer = new CServer(GetServicePath() + "\\config\\employees.xml", GetServicePath() + "\\Log\\server_logs.txt");
	pServer->GetLogger()->SetLoggerType(GetServicePath() + "\\config\\server.xml");
	if(GetServiceParameters())
	{
		if(argc == 1)
		{
			ServiceTable[0].lpServiceName = reinterpret_cast<LPSTR>(const_cast<char*>(sServiceName.c_str()));
			ServiceTable[0].lpServiceProc = reinterpret_cast<LPSERVICE_MAIN_FUNCTION>(Main);
			StartServiceCtrlDispatcher(ServiceTable);
			printf("employee_manager_server.exe [install|uninstall|stop|start|restart]");
		}
		else 
		{
			if(strcmp(argv[argc-1], csInstallCmd.c_str()) == 0)
			{
				Install(argv[0]);
			}
			else if(strcmp(argv[argc-1], csUninstallCmd.c_str()) == 0)
			{
				Uninstall();
			}
			else if(strcmp(argv[argc-1], csStartCmd.c_str()) == 0)
			{
				Start();
			}
			else if(strcmp(argv[argc-1], csStopCmd.c_str()) == 0)
			{
				Stop();
			}
			else if(strcmp(argv[argc-1], csRestartCmd.c_str()) == 0)
			{
				Stop();
				Start();
			}
			else
			{
				printf("Unknown command.\n");
				printf("employee_manager_server.exe [install|uninstall|stop|start|restart]");
			}
		}
	}
}