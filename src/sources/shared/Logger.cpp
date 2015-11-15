#include "Logger.h"
#include "..\employee_manager_server\Service.h"

CLogger::~CLogger(void)
{
}

bool CLogger::SetLoggerType(const string& csConfigXmlFilePath)
{
	bool bOk = true;
	CXmlModel XmlModel(csConfigXmlFilePath);
	do
	{

		if(bOk = XmlModel.Create(false))
		{
			const CNodeElement* pRoot = XmlModel.GetRootNode();
			int iIndex = -1;
			bOk = pRoot->HasChildWithName(CXmlKeyWords::csConfiguration, iIndex);
			if(iIndex == -1)
			{
				m_bIsDebugLogger = false;
				bOk = false;
				break;
			}
			pRoot = pRoot->GetChild(iIndex);
			string Type = pRoot->GetAttributeValue(CXmlKeyWords::csType);
			if(Type == CXmlKeyWords::csProduction)
			{
				m_bIsDebugLogger = false;
				break;
			}
			if(Type == CXmlKeyWords::csDebug)
			{
				m_bIsDebugLogger = true;
				break;
			}
			m_bIsDebugLogger = false;
			bOk = false;
		}
	}while(false);
	return bOk;
}

bool CLogger::AppendLogFile()
{
	CreateDirectory("Log", NULL);
	fopen_s(&m_fLogFile, m_sLogFilePath.c_str(), "a+");
	if(m_fLogFile != NULL)
	{
		return true;
	}
	return false;
}

void CLogger::CloseLogFile()
{
	fclose(m_fLogFile);
}

void CLogger::WriteLog(bool isDebug, const string& csMsg, bool bConsoleOutput)
{
	string sType;
	if(isDebug)
	{
		sType = "[0]";
	}
	else
	{
		sType = "[1]";
	}
	if(bConsoleOutput)
	{
		printf("%s\n", csMsg.c_str());
	}

	if(AppendLogFile())
	{
		DWORD dwThreadId = GetCurrentThreadId();
		const size_t iBuffSize = 300;
		char buff[iBuffSize];
		const time_t Seconds = time(NULL);
		tm TimeInfo;
		localtime_s(&TimeInfo, &Seconds);
		char* pcTimeDateFormat = "%B %d %Y %H:%M:%S";
		strftime(buff, iBuffSize, pcTimeDateFormat, &TimeInfo);
		fprintf(m_fLogFile, "%d\t%s\t%s\t%s\n", dwThreadId, buff, sType.c_str(), csMsg.c_str());
		CloseLogFile();
	}
	else
	{
		printf("Problem with log-file!\n");
	}
}

void CLogger::WriteProductionLog(const string& csMsg, bool bConsoleOutput)
{
	WriteLog(false, csMsg, bConsoleOutput);
}

void CLogger::WriteDebugLog(const string& csMsg, bool bConsoleOutput)
{
	if(m_bIsDebugLogger)
	{
		WriteLog(true, csMsg, bConsoleOutput);
	}
}