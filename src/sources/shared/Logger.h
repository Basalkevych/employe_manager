#pragma once
#include <string>
using std::string;
#include "XmlModel.h"
#include "XmlKeyWords.h"
#include <ctime>

class CLogger
{
public:
	CLogger(const string& csLogFilePath):m_sLogFilePath(csLogFilePath) {};
	virtual ~CLogger(void);

private:
	void WriteLog(bool isDebug,const string& csMsg, bool bConsoleOutput = false);

protected:
	bool IsDebugType() { return m_bIsDebugLogger; };
	bool AppendLogFile();
	void CloseLogFile();

public:
	bool SetLoggerType(const string& csConfigXmlFilePath);
	void WriteProductionLog(const string& csMsg, bool bConsoleOutput = false);
	void WriteDebugLog(const string& csMsg, bool bConsoleOutput = false);

private:
	FILE* m_fLogFile;
	string m_sLogFilePath;
	bool m_bIsDebugLogger;
};

