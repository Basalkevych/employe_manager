#pragma once
#include "Server.h"
#include <string>
using std::string;

class CService
{
public:
	enum ServStatus { ALREADY_STOPPED, STOPPED, UNKNOWN };	
	static void Launch(int argc, char** argv);
	static string GetServicePath();

private:

	CService(void);
	~CService(void);

	static bool Init();
	static void Main(int argc, char** argv);
	static void Install(const char* argv0);
	static void Uninstall();
	static void Start();
	static void ControlHandler(DWORD request);
	static ServStatus IsStopped(string& sMsg);
	static void Stop();
	static bool GetServiceParameters();

public:

private:

	static SERVICE_STATUS_HANDLE StatusHandle;
	static SERVICE_STATUS ServiceStatus;
	static string sServiceName;
	static string sServiceShownName;
	static string sServiceDescription;
	static string sServicePath;
	static SERVICE_TABLE_ENTRY ServiceTable[1];
	static CServer* pServer;

	static const string csInstallCmd;
	static const string csUninstallCmd;
	static const string csStartCmd;
	static const string csStopCmd;
	static const string csRestartCmd;
};

