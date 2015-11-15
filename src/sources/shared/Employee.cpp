#pragma once
#include "Employee.h"

CEmployee::CEmployee(const string &csId, const vector<string> &cvsChildren,
	const string &csChildren, const string &csSurname, const string &csFirstName,
	const string &csSecondName, const string &csPosition, const string &csRoomPlace,
	const string &csBirthDate, const string &csProject, const string &csEnglishLevel,
	const string &csPhone, const string &csEmail, const string &csSkype) :
	m_csId(csId), m_cvsChildren(cvsChildren), m_csChildren(csChildren), m_csSurname(csSurname),
	m_csFirstName(csFirstName), m_csSecondName(csSecondName), m_csPosition(csPosition),
	m_csRoomPlace(csRoomPlace), m_csBirthDate(csBirthDate), m_csProject(csProject),
	m_csEnglishLevel(csEnglishLevel), m_csPhone(csPhone), m_csEmail(csEmail), m_csSkype(csSkype)
{
}

CEmployee::~CEmployee(void)
{
}

bool CEmployee::HasChild(const string &csChildId) const
{
	vector<string> vsChildren = GetChildren();
	for(size_t i = 0; i < vsChildren.size(); ++i)
	{
		if(vsChildren[i] == csChildId)
		{
			return true;
		}
	}
	return false;
}

bool CEmployee::HasParent(const string &csParentId) const
{
	vector<string> vsParents = GetParents();
	for(size_t i = 0; i < vsParents.size(); ++i)
	{
		if(vsParents[i] == csParentId)
		{
			return true;
		}
	}
	return false;
}
