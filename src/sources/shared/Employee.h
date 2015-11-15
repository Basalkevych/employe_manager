#pragma once
#include <vector>
#include <string>
using std::vector;
using std::string;

class CEmployee
{
public:
	CEmployee(const string &csId, const vector<string> &cvsChildren,
		const string &csChildren, const string &csSurname, const string &csFirstName,
		const string &csSecondName, const string &csPosition, const string &csRoomPlace,
		const string &csBirthDate, const string &csProject, const string &csEnglishLevel,
		const string &csPhone, const string &csEmail, const string &csSkype);

	~CEmployee(void);

	string GetId() const { return m_csId; }
	vector<string> GetChildren() const { return m_cvsChildren; }
	string GetChildrenString() const { return m_csChildren; }
	string GetSurname() const { return m_csSurname; }
	string GetFirstName() const { return m_csFirstName; }
	string GetSecondName() const { return m_csSecondName; }
	string GetPosition() const { return m_csPosition; }

	string GetRoomPlace() const { return m_csRoomPlace; }
	string GetBirthDate() const { return m_csBirthDate; }
	string GetProject() const { return m_csProject; }
	string GetEnglishLevel() const { return m_csEnglishLevel; }
	string GetPhone() const { return m_csPhone; }
	string GetEmail() const { return m_csEmail; }
	string GetSkype() const { return m_csSkype; }
	vector<string> GetParents() const { return m_cvsParents; }
	bool HasChild(const string &csChildId) const;
	bool HasParent(const string &csParentId) const ;

protected:

private:
	CEmployee& operator=(const CEmployee&);
	friend class CEmployeesContainer;
	
public:

protected:

private:
	const string m_csId;
	const vector<string> m_cvsChildren;
	const string m_csChildren;
	const string m_csSurname;
	const string m_csFirstName;
	const string m_csSecondName;
	const string m_csPosition;

	const string m_csRoomPlace;
	const string m_csBirthDate;
	const string m_csProject;
	const string m_csEnglishLevel;
	const string m_csPhone;
	const string m_csEmail;
	const string m_csSkype;

	vector<string> m_cvsParents;

};

