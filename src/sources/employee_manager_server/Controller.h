#pragma once
#include "../shared/Employee.h"
#include "../shared/EmployeesContainer.h"
#include "../shared/GraphBuilder.h"
#include "../shared/Table.h"
#include "../shared/Commands.h"
#include <set>
using std::set;

class CController
{
public:
	CController(const string& sXmlFilePath);
	~CController();

	CTable* Execute(int argc, char* argv[], string& sErrString);
	static string GetActualParameters(const string &csData);
	bool IsOk() { return m_pEmployeesContainer->IsOK(); }
	string GetErrorString() { return m_pEmployeesContainer->GetErrorMsg(); }
	
private:
	CTable* GetEmployees(int argc, char* argv[], string& sErrString);
	CTable* GetEmployeeData(int argc, char* argv[], string& sErrString);
	CTable* GetInferiors(int argc, char* argv[], string& sErrString);
	CTable* GetBosses(int argc, char* argv[], string& sErrString);
	CTable* GetColleagues(int argc, char* argv[], string& sErrString);
	CTable* GetEmployeesRelations(int argc, char* argv[], string& sErrString);
	bool ValidateConfiguration(bool bPrintOkMsg = false);
	CTable* ValidateConfiguration(string& sErrString);
	
public:

private:
	CEmployeesContainer* m_pEmployeesContainer;
};

