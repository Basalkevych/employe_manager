#pragma once
#include "NodeElement.h"
#include <cassert>
class CXmlModel
{
public:
	CXmlModel(const string& csData, bool bIsFileName = true);
	~CXmlModel(void);
	bool Create(bool bPrintLog = true);
	vector<const CNodeElement*> Get2LevelNodes() const;
	const CNodeElement* GetRootNode() const;

private:
	bool OpenFile();
	void ReadFile();
	void GoBack(const string& csTagName, CNodeElement*& pCurrElem);
	void ClearVersionAndComments(bool bPrintLog);
	bool NameIsOk(string sName, bool bPrintLog);
	bool AttributeIsOk(const string& sAttribute, bool bPrintLog);
	void CheckNames(CNodeElement*& pRoot, bool bPrintLog);
	static void CleanMemory(CNodeElement*& pRoot);

public:

private:
	CNodeElement* m_pRoot;
	string m_sFileName;
	FILE* m_pFile;
	bool m_bModelIsOk;
	string m_sText;

};

