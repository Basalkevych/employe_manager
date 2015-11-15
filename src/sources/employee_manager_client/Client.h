#pragma once
#include "winsock2.h"
#include "../shared/XmlModel.h"
#include "../shared/XmlKeyWords.h"
#include "../shared/SocketConnection.h"
#include "../shared/Table.h"
#include "../shared/Commands.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

class CClient : public CSocketConnection
{

public:
	CClient(const string& csLogFilePath): CSocketConnection(csLogFilePath){}
	bool Init(const string& csConfigXmlPath);
	void Launch(int argc, char**& argv);
	~CClient(void);

private:
	bool SendRequest(int argc,  char** argv);
	bool ReceiveResponse(string& sData);
	static void InitHelp();
	static string GetHelpInfoLine(size_t iLineIndex);
	static void ShowHelp();
	static bool HasParams(const string& csData, const string& sParam1, bool bStrict=false);
	static bool HasParams(const string& csData, const string& sParam1, const string& sParam2);
	static bool StartsWith(const string& csData, const string& sParam);
	static bool CheckCommand(const string& csData, const string& sParams);

public:

private:
	string m_sConnectionIp;
	static vector<string> vsHelpInfo;

};

