#pragma once
#include "winsock2.h"
#include "../shared/XmlModel.h"
#include "../shared/XmlKeyWords.h"
#include "../shared/SocketConnection.h"
#include "../employee_manager_server/Controller.h"
#include "../shared/Logger.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

class CServer : public CSocketConnection
{
public:
	CServer(const string& csServicesXmlPath,const string& csLogFilePath);
	~CServer(void);
	bool Init(const string& csServicesXmlPath, const string& csConfigXmlPath);

	void Launch();

private:
	bool ReceiveRequest(int& argc, char**& argv);
	bool SendResponce(const string& sData);

private:
	CController* m_pController;

};

