#include "XmlModel.h"

CXmlModel::CXmlModel(const string& csData, bool bIsFileName): m_pRoot(NULL), m_bModelIsOk(true) 
{
	if(bIsFileName)
	{
		m_sFileName = csData;
	}
	else
	{
		m_sText = csData;
	}
}

void CXmlModel::CleanMemory(CNodeElement*& pRoot)
{
	for(size_t i = 0; i < pRoot->GetNumberOfChildren(); ++i)
	{
		CNodeElement* pChild = pRoot->GetChild(i);
		if(pChild != NULL)
		{
			CleanMemory(pChild);
		}
	}
	delete pRoot;
	pRoot = NULL;
}

void CXmlModel::CheckNames(CNodeElement*& pRoot, bool bPrintLog)
{
	for(size_t i = 0; i < pRoot->GetNumberOfChildren(); ++i)
	{
		CNodeElement* pChild = pRoot->GetChild(i);
		if(pChild != NULL)
		{
			CheckNames(pChild, bPrintLog);
		}
	}
	if(!NameIsOk(pRoot->GetName(), bPrintLog))
	{
		if(bPrintLog)
		{
			printf("Bad tag name: <%s>\n\n", pRoot->GetName().c_str());
			int iLevel = 0;
			CNodeElement::PrintStack(pRoot, iLevel);
		}
	}
	map<string, string> mpAttributes = pRoot->GetAttributes();
	for(map<string, string>::iterator it = mpAttributes.begin(); it != mpAttributes.end(); ++it)
	{
		if(!AttributeIsOk(it->first, bPrintLog))
		{
			if(bPrintLog)
			{
				printf("Bad attribute name: %s\n\n", it->first.c_str());
				int iLevel = 0;
				CNodeElement::PrintStack(pRoot, iLevel);
			}
		}
	}
}

CXmlModel::~CXmlModel(void)
{
	if(m_pRoot != NULL)
	{
		CleanMemory(m_pRoot);
	}
}

void CXmlModel::GoBack(const string& csTagName, CNodeElement*& pCurrElem)
{
	if(pCurrElem == NULL)
	{
		pCurrElem = new CNodeElement(csTagName);
		m_pRoot = pCurrElem;
	}
	else
	{
		CNodeElement* pTemp = new CNodeElement(csTagName);
		pTemp->SetParent(pCurrElem);
		pCurrElem->AddChild(pTemp);
		pCurrElem = pTemp;
	}
}

bool CXmlModel::OpenFile()
{
	fopen_s(&m_pFile,m_sFileName.c_str(), "r");
	return (m_pFile != NULL);
}

void CXmlModel::ReadFile()
{
	m_sText.reserve(5000);
	char cCurrSymb;
	while (fscanf_s(m_pFile, "%c", &cCurrSymb, sizeof(char)) > 0)
	{
		if(cCurrSymb != '\n' && cCurrSymb != '\t')
		{
			m_sText += cCurrSymb;
		}
	}
	fclose(m_pFile);
}

void CXmlModel::ClearVersionAndComments(bool bPrintLog)
{
	size_t iStart;
	while((iStart=m_sText.find("<?")) != string::npos)
	{
		size_t iEnd = m_sText.find("?>",iStart + 1);
		if(iEnd ==string::npos)
		{
			if(bPrintLog)
			{
				printf("Bad version tag\n");
			}
			m_bModelIsOk = false;
			return;
		}
		m_sText.erase(iStart, iEnd - iStart + 2);
	}

	while((iStart=m_sText.find("<!--")) != string::npos)
	{
		size_t iEnd = m_sText.find("-->",iStart + 1);
		if(iEnd ==string::npos)
		{
			if(bPrintLog)
			{
				printf("Bad comment tag\n");
			}
			m_bModelIsOk = false;
			return;
		}
		m_sText.erase(iStart, iEnd - iStart + 3);
	}
}
  
bool CXmlModel::NameIsOk(string sName, bool bPrintLog)
{
	if(sName.front() == ' ' || iswdigit(sName.front()))
	{
		if(bPrintLog)
		{
			printf("Starts with forbidden symbol\n");
		}
		return false;
	}
	while(sName.back() == ' ')
	{
		sName.pop_back();
	}
	for(size_t i = 0; i < sName.size(); ++i)
	{
		if(iswpunct(sName[i]) && sName[i] != '.' && sName[i] !='_')
		{
			if(bPrintLog)
			{
				printf("Forbidden symbol\n");
			}
			return false;
		}
	}
	return true;
}

bool CXmlModel::AttributeIsOk(const string& sAttribute, bool bPrintLog)
{
	if(sAttribute.front() == ' ' || iswdigit(sAttribute.front()))
	{
		if(bPrintLog)
		{
			printf("Starts with forbidden symbol\n");
		}
		return false;
	}
	for(size_t i = 0; i < sAttribute.size(); ++i)
	{
		if(iswpunct(sAttribute[i]) && sAttribute[i] != '.' && sAttribute[i] != '_')
		{
			if(bPrintLog)
			{
				printf("Forbidden symbol\n");
			}
			return false;
		}
	}
	return true;
}

bool CXmlModel::Create(bool bPrintLog)
{
	if(!m_sFileName.empty())
	{
		if( !OpenFile())
		{
			if(bPrintLog)
			{
				printf("XML-file %s NOT found!\n", m_sFileName.c_str());
			}
			return false;
		}
		ReadFile();
	}
	ClearVersionAndComments(bPrintLog);

	CNodeElement* pCurrElem = NULL;
	m_pRoot = pCurrElem;
	string sAtrName, sAtrValue, sOpenTag, sCloseTag;
	if(m_sText[0] != '<')
	{
		if(bPrintLog)
		{
			printf("XML-file %s does not start with root tag!\n", m_sFileName.c_str());
		}
		m_bModelIsOk = false;
		return false;
	}

	int iStatus = 0; //status of finite machine, [0..8]

	for(size_t i = 1; i < m_sText.size(); ++i)
	{
		char c = m_sText[i];
		switch(iStatus)
		{
		case 0:
			if(c == '/')
			{
				iStatus = 1;
			}
			else
				if(c != '/')
				{
					sOpenTag +=c;
					iStatus = 3;
				}
				else
				{
					if(bPrintLog)
					{
						printf("Unhandled: %c\niStatus: %d\nPosition: %d\n", c, iStatus, i);
					}
					assert(false);
				}
			break;
		case 1:
			if(c != '>')
			{
				sCloseTag += c;
				iStatus = 1;
			}
			else
			{
				if(c == '>')
				{
					while(sCloseTag.back() == ' ')
					{
						sCloseTag.pop_back();
					}
					if(pCurrElem == NULL)
					{
						if(bPrintLog)
						{
							printf("Tag </%s> is unwanted!\n", sCloseTag.c_str());
						}
							m_bModelIsOk = false;
							return false;
					}
					else if(pCurrElem->GetName() != sCloseTag)
					{
						if(bPrintLog)
						{
							printf("Tag <%s> is not closed or tag </%s> is unwanted!\n\n", pCurrElem->GetName().c_str(), sCloseTag.c_str());
						int iLevel = 0;
						CNodeElement::PrintStack(pCurrElem, iLevel);
						}
					m_bModelIsOk = false;
					return false;
					}
					else
					{
						pCurrElem = pCurrElem->GetParent();
						sCloseTag.clear();
						iStatus = 2;
					}
				}
				else
				{
					if(bPrintLog)
					{
						printf("Unhandled: %c\niStatus: %d\nPosition: %d\n", c, iStatus, i);
					}
					assert(false);
				}
			}
			break;
		case 2:
			if(c != '<')
			{
				pCurrElem->AppendText(c);
				iStatus = 2;
			}
			else
				if(c == '<')
				{
					iStatus = 0;
				}
				else
				{
					if(bPrintLog)
					{
						printf("Unhandled: %c\niStatus: %d\nPosition: %d\n", c, iStatus, i);
					}
					assert(false);
				}
			break;
		case 3:
			if(c != '>' && c != ' ' && c != '/')
			{
				sOpenTag += c;
				iStatus = 3;
			}
			else
				if(c == '>')
				{
					GoBack(sOpenTag, pCurrElem);
					sOpenTag.clear();
					iStatus = 2;
				}
				else
					if(c == ' ')
					{
						GoBack(sOpenTag, pCurrElem);
						sOpenTag.clear();
						iStatus = 4;
					}
					else
						if(c == '/')
						{
							GoBack(sOpenTag, pCurrElem);
							sOpenTag.clear();
							iStatus = 8;
						}
						else
						{
							if(bPrintLog)
							{
								printf("Unhandled: %c\niStatus: %d\nPosition: %d\n", c, iStatus, i);
							}
							assert(false);
						}
			break;
		case 4:
			if(c == ' ')
			{
				iStatus = 4;
			}
				else
					if(c == '/')
					{
						iStatus = 8;
					}
					else
						if(c == '>')
						{
							iStatus = 2;
						}
						else
						{
							sAtrName +=c;
							iStatus = 5;
						}
			break;
		case 5:
			if(c != '=')
			{
				sAtrName +=c;
				iStatus = 5;
			}
			else
				if(c == '=')
				{
					iStatus = 6;
				}
				else
				{
					if(bPrintLog)
					{
						printf("Unhandled: %c\niStatus: %d\nPosition: %d\n", c, iStatus, i);
					}
					assert(false);
				}
			break;
		case 6:
			if(c == ' ')
			{
				iStatus = 6;
			}
			else
				if(c == '\"')
				{
					iStatus = 7;
				}
			break;
		case 7:
			if(c != '\"')
			{
				sAtrValue += c;
				iStatus = 7;
			}
			else
				if(c == '\"')
				{
					pCurrElem->AddAttribute(sAtrName, sAtrValue);
					sAtrName.clear();
					sAtrValue.clear();
					iStatus = 4;
				}
				else
				{
					if(bPrintLog)
					{
						printf("Unhandled: %c\niStatus: %d\nPosition: %d\n", c, iStatus, i);
					}
					assert(false);
				}
			break;
		case 8:
			if(c == '>')
			{
				pCurrElem = pCurrElem->GetParent();
				iStatus = 2;
			}
			else
			{
				if(bPrintLog)
				{
					printf("Unhandled: %c\niStatus: %d\nPosition: %d\n", c, iStatus, i);
				}
				assert(false);
			}
			break;
		}
	}
	CheckNames(m_pRoot, bPrintLog);
	return true;
}	

vector<const CNodeElement*> CXmlModel::Get2LevelNodes() const
{
	vector<const CNodeElement*> vResult;
	for(size_t i = 0; i < m_pRoot->GetNumberOfChildren(); ++i)
	{
		CNodeElement* pCurr = m_pRoot->GetChild(i);
		if(pCurr != NULL)
		{
			vResult.push_back(pCurr);
		}
	}
	return vResult;
}

const CNodeElement* CXmlModel::GetRootNode() const
{
	return m_pRoot;
}

