#include "NodeElement.h"


const string CNodeElement::csNoSuchData = "<Empty>"; //Not xml-tag.

CNodeElement::CNodeElement(const string& csName): m_sName(csName), m_pParent(NULL)
{
}

bool CNodeElement::HasChildWithName(const string& csName, int& iFoundIndex) const
{
	iFoundIndex = -1;
	for(size_t i = 0; i < m_vpChildren.size(); ++i)
	{
		if(m_vpChildren[i]->GetName() == csName)
		{
			iFoundIndex = static_cast<int>(i);
			return true;
		}
	}
	return false;
}

string CNodeElement::GetAttributeValue(const string& csAttributeName) const
{
	if(m_mpAttributes.find(csAttributeName) == m_mpAttributes.end())
	{
		return CNodeElement::csNoSuchData;
	}
	else
		return m_mpAttributes.at(csAttributeName);
}

CNodeElement* const CNodeElement::GetChild(size_t iIndex) const
{
	if(iIndex > m_vpChildren.size())
	{
		return NULL;
	}
	else
	{
		return m_vpChildren[iIndex];
	}
}

void CNodeElement::PrintStack(const CNodeElement* pElem, int& iLevel)
{
	CNodeElement* pParent = pElem->GetParent();
	if(pParent != NULL)
	{
		PrintStack(pParent, ++iLevel);
	}
	printf("<%s", pElem->GetName().c_str());
	map<string, string> mpAttributes = pElem->GetAttributes();
	for(map<string, string>::iterator it = mpAttributes.begin(); it != mpAttributes.end(); ++it)
	{
		printf(" %s=\"%s\"", it->first.c_str(), it->second.c_str());
	}
	printf(">\n");
	if(iLevel != 0)
	{
		printf("  |\n");
		printf("  V\n");
		--iLevel;
	}
	else
	{
		printf("\n");
	}
}

CNodeElement::~CNodeElement(void)
{
}
