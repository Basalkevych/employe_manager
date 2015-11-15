#pragma once
#include "../shared/XmlKeyWords.h"
#include "EmployeesContainer.h"
#include <set>
using std::set;

const string CEmployeesContainer::csEmptyProrerty = "<Empty>";

CEmployeesContainer::CEmployeesContainer(const string &csFileName)  : m_sFileName(csFileName), m_bBadFile(false),
	m_sErrorMsg(""), m_iCurrentTextPos(0) {};

CEmployeesContainer::~CEmployeesContainer()
{
}

void CEmployeesContainer::CheckSelfRelation()
{
	string sMyId;
	for(size_t i = 0; i < m_vEmployees.size(); ++i)
	{
		sMyId = m_vEmployees[i].GetId();
		if(m_vEmployees[i].HasChild(sMyId))
		{
			m_sErrorMsg += "\nSelf-relation in employee with id = " + sMyId;
			m_bBadFile = true;
		}
	}
}

bool CEmployeesContainer::CycleExists()
{
	bool bResult = false;
	CGraphBuilder* GraphBuilder = new CGraphBuilder(m_vEmployees, this);
	GraphBuilder->Init();
	GraphBuilder->CreateGraphModel();
	vector<string> vsCycle;
	if(GraphBuilder->CycleExist(vsCycle))
	{
		bResult = true;
		m_bBadFile = true;
		m_sErrorMsg += "\nCycle in relations found: ";
		m_sErrorMsg += vsCycle.front();
		for(size_t i = 1; i < vsCycle.size(); ++i)
		{
			m_sErrorMsg += " -> " + vsCycle[i];
		}
		m_sErrorMsg += "\n";
	}
	delete GraphBuilder;
	return bResult;
}

vector<string> CEmployeesContainer::ParseIds(const string &csIds)
{
	vector<string> vsResult;
	string sCurrentId("");
	size_t iCurrPos = 0;
	while(iCurrPos < csIds.size())
	{
		if(csIds[iCurrPos] == ',')
		{
			if(!sCurrentId.empty())
			{
				vsResult.push_back(sCurrentId);
				sCurrentId.clear();
			}
		}
		else
		{
			sCurrentId += csIds[iCurrPos];
		}
		++iCurrPos;
	}
	if(!sCurrentId.empty())
	{
		vsResult.push_back(sCurrentId);
	}

	return vsResult;
}

bool CEmployeesContainer::EmployeeExists(const string &csEmployeeId, int &iEmployeeIndex) const
{
	iEmployeeIndex = -1;
	for(size_t i = 0; i < m_vEmployees.size(); ++i)
	{
		if(m_vEmployees[i].GetId() == csEmployeeId)
		{
			iEmployeeIndex = static_cast<int>(i);
			return true;
		}
	}
	return false;
}

bool CEmployeesContainer::AddParentsInfo()
{
	for(size_t i = 0; i < m_vEmployees.size(); ++i)
	{
		for(size_t j = 0; j < m_vEmployees[i].m_cvsChildren.size(); ++j)
		{
			int iIndex = -1;
			if(EmployeeExists(m_vEmployees[i].m_cvsChildren[j], iIndex))
			{
				m_vEmployees[iIndex].m_cvsParents.push_back(m_vEmployees[i].GetId());
			}
			else
			{
				m_bBadFile = true;
				m_sErrorMsg += "\nNo employee with id = " + m_vEmployees[i].m_cvsChildren[j];
			}
		}
	}
	return m_bBadFile;
}

CEmployee* CEmployeesContainer::GetEmployeeById(const string &csEmployeeId)
{
	int iFoundIndex = -1;
	if(EmployeeExists(csEmployeeId, iFoundIndex))
	{
		return &m_vEmployees[iFoundIndex];
	}
	else
	{
		return NULL;
	}
}

bool CEmployeesContainer::LoadXml()
{
	CXmlModel* xmlModel = new CXmlModel(m_sFileName);
	bool bOk = xmlModel->Create();
	if(!bOk)
	{
		m_bBadFile = true;
		return false;
	}
	vector<const CNodeElement*> vEmployees = xmlModel->Get2LevelNodes();
	for(size_t i = 0; i < vEmployees.size(); ++i)
	{
		string sId;
		string sChildren;
		vector<string> vsChildrenIds;

		string sSurname;
		string sFirstName;
		string sSecondName;
		string sPosition;

		string sRoomPlace = CNodeElement::csNoSuchData;;
		string sBirthDate = CNodeElement::csNoSuchData;;
		string sProject = CNodeElement::csNoSuchData;;
		string sEnglishLevel = CNodeElement::csNoSuchData;;
		string sPhone = CNodeElement::csNoSuchData;;
		string sEmail = CNodeElement::csNoSuchData;;
		string sSkype = CNodeElement::csNoSuchData;;

		sId = vEmployees[i]->GetAttributeValue(CXmlKeyWords::csId);
		if(sId == CNodeElement::csNoSuchData)
		{
			m_bBadFile = true;
			m_sErrorMsg += "\nEmployee #" + std::to_string(i+1) + " has no id attribute!\n";
			delete xmlModel;
			return false;
		}
		sChildren = vEmployees[i]->GetAttributeValue(CXmlKeyWords::csChildrenList);
		if(sChildren == CNodeElement::csNoSuchData)
		{
			sChildren.clear();
		}
		vsChildrenIds = CEmployeesContainer::ParseIds(sChildren);
		int iIndex = -1;
		if(vEmployees[i]->HasChildWithName(CXmlKeyWords::csSurname, iIndex))
		{
			sSurname = vEmployees[i]->GetChild(iIndex)->GetText();
		}
		else
		{
			m_bBadFile = true;
			m_sErrorMsg += "\nEmployee with id = " + sId + " has no surname!\n";
			delete xmlModel;
			return false;
		}
		if(vEmployees[i]->HasChildWithName(CXmlKeyWords::csFirstName, iIndex))
		{
			sFirstName = vEmployees[i]->GetChild(iIndex)->GetText();
		}
		else
		{
			m_bBadFile = true;
			m_sErrorMsg += "\nEmployee with id = " + sId + " has no first_name!\n";
			delete xmlModel;
			return false;
		}
		if(vEmployees[i]->HasChildWithName(CXmlKeyWords::csSecondName, iIndex))
		{
			sSecondName = vEmployees[i]->GetChild(iIndex)->GetText();
		}
		else
		{
			m_bBadFile = true;
			m_sErrorMsg += "\nEmployee with id = " + sId + " has no second_name!\n";
			delete xmlModel;
			return false;
		}
		if(vEmployees[i]->HasChildWithName(CXmlKeyWords::csPosition, iIndex))
		{
			sPosition = vEmployees[i]->GetChild(iIndex)->GetText();
		}
		else
		{
			m_bBadFile = true;
			m_sErrorMsg += "\nEmployee with id = " + sId + " has no position!\n";
			delete xmlModel;
			return false;
		}
		if(vEmployees[i]->HasChildWithName(CXmlKeyWords::csProperties, iIndex))
		{
			CNodeElement* pProperties = vEmployees[i]->GetChild(iIndex);
			if(pProperties->HasChildWithName(CXmlKeyWords::csRoomPlace, iIndex))
			{
				sRoomPlace = pProperties->GetChild(iIndex)->GetText();
			}
			if(pProperties->HasChildWithName(CXmlKeyWords::csBirthDate, iIndex))
			{
				sBirthDate = pProperties->GetChild(iIndex)->GetText();
			}
			if(pProperties->HasChildWithName(CXmlKeyWords::csProject, iIndex))
			{
				sProject = pProperties->GetChild(iIndex)->GetText();
			}
			if(pProperties->HasChildWithName(CXmlKeyWords::csEnglishLevel, iIndex))
			{
				sEnglishLevel = pProperties->GetChild(iIndex)->GetText();
			}

			if(pProperties->HasChildWithName(CXmlKeyWords::csPhone, iIndex))
			{
				sPhone = pProperties->GetChild(iIndex)->GetText();
			}
			if(pProperties->HasChildWithName(CXmlKeyWords::csEmail, iIndex))
			{
				sEmail = pProperties->GetChild(iIndex)->GetText();
			}
			if(pProperties->HasChildWithName(CXmlKeyWords::csSkype, iIndex))
			{
				sSkype = pProperties->GetChild(iIndex)->GetText();
			}
		}
		CEmployee Employee(sId, vsChildrenIds, sChildren, sSurname, sFirstName,
			sSecondName, sPosition, sRoomPlace, sBirthDate, sProject, sEnglishLevel, sPhone,
			sEmail, sSkype);
		m_vEmployees.push_back(Employee);
	}
	delete xmlModel;
	AddParentsInfo();
	CheckSelfRelation();
	CycleExists();
	return true;
}


