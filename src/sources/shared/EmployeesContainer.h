#pragma once
#include "../shared/Employee.h"
#include "../shared/GraphBuilder.h"
#include "../shared/NodeElement.h"
#include "../shared/XmlModel.h"
#include <vector>
#include <string>
using std::vector;
using std::string;
class CEmployeesContainer
{
public:
	CEmployeesContainer(const string &csFileName);
	~CEmployeesContainer();

	bool LoadXml();
	bool IsOK() const { return !m_bBadFile; }
	string GetErrorMsg() const { return m_sErrorMsg; }
	vector<CEmployee> GetAllEmployees() const { return m_vEmployees; }
	CEmployee* GetEmployeeById(const string &ñsEmployeeId);
	static vector<string> ParseIds(const string &ñsIds);
	void CheckSelfRelation();
	bool CycleExists();
	
private:
	bool EmployeeExists(const string &ñsEmployeeId, int &iEmployeeIndex) const;
	bool AddParentsInfo();
	CEmployeesContainer& operator=(const CEmployeesContainer&);

public:
	static const string csEmptyProrerty;

protected:

private:
	vector<CEmployee> m_vEmployees;
	const string m_sFileName;
	string m_sText;
	FILE* m_pFile;
	bool m_bBadFile;
	string m_sErrorMsg;
	size_t m_iCurrentTextPos;

};

