#pragma once
#include "Controller.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

CController::CController(const string& sXmlFilePath)
{
	m_pEmployeesContainer = new CEmployeesContainer(sXmlFilePath);
	m_pEmployeesContainer->LoadXml();
}

CController::~CController()
{
	if(m_pEmployeesContainer != NULL)
	{
		delete m_pEmployeesContainer;
	}
}

CTable* CController::GetEmployeeData(int argc, char* argv[], string& sErrString)
{
	CTable* RezTable = NULL;
	sErrString.clear();

	vector<string> vsTableHead;
	vsTableHead.push_back(CTable::csEmployeeNameTitle);
	vsTableHead.push_back(CTable::csPropertyNameTitle);
	vsTableHead.push_back(CTable::csPropertyValueTitle);
	vector<size_t> viColumnSize;
	viColumnSize.push_back(30);
	viColumnSize.push_back(14);
	viColumnSize.push_back(23);
	RezTable = new CTable(vsTableHead, viColumnSize);

	vector<CEmployee> vEmployees;
	if(argc == 2)
	{
		for(size_t i = 0; i < m_pEmployeesContainer->GetAllEmployees().size(); ++i)
		{
			vEmployees.push_back(m_pEmployeesContainer->GetAllEmployees()[i]);
		}
	}
	else
	{
		vector<string> vsIds = CEmployeesContainer::ParseIds(GetActualParameters(argv[2]));
		CEmployee* ServiceElem = NULL;
		for(size_t i = 0; i < vsIds.size(); ++i)
		{
			ServiceElem = m_pEmployeesContainer->GetEmployeeById(vsIds[i]);
			if(ServiceElem != NULL)
			{
				vEmployees.push_back(*ServiceElem);
			}
			else
			{
				continue;
			}
		}
	}
	RezTable->CreateDataTable(vEmployees);
	return RezTable;
}

CTable* CController::GetInferiors(int argc, char* argv[], string& sErrString)
{
	CTable* RezTable = NULL;
	sErrString.clear();

	vector<string> vsTableHead;
	vsTableHead.push_back(CTable::csEmployeeIdTitle);
	vsTableHead.push_back(CTable::csEmployeeNameTitle);
	vsTableHead.push_back(CTable::csEmployeePositionTitle);
	vector<size_t> viColumnSize;
	viColumnSize.push_back(10);
	viColumnSize.push_back(30);
	viColumnSize.push_back(27);
	RezTable = new CTable(vsTableHead, viColumnSize);

	vector<string> vsIds = CEmployeesContainer::ParseIds(GetActualParameters(argv[2]));
	CEmployee* ServiceElem = m_pEmployeesContainer->GetEmployeeById(vsIds[0]);
	if(ServiceElem == NULL)
	{
		return RezTable;
	}
	vector<CEmployee> vEmployees;
	for(size_t i = 0; i < ServiceElem->GetChildren().size(); ++i)
	{
		string sChildServiceId = ServiceElem->GetChildren()[i];
		CEmployee* ChildServiceElem = m_pEmployeesContainer->GetEmployeeById(sChildServiceId);
		if(ChildServiceElem == NULL)
		{
			sErrString += "Service has child with id = " + sChildServiceId + ", but such does not exist!";
			delete RezTable;
			RezTable = NULL;
			return RezTable;
		}
		vEmployees.push_back(*ChildServiceElem);
	}
	RezTable->CreateEmployeesTable(vEmployees);
	return RezTable;
}

CTable* CController::GetBosses(int argc, char* argv[], string& sErrString)
{
	CTable* RezTable = NULL;
	sErrString.clear();

	vector<string> vsTableHead;
	vsTableHead.push_back(CTable::csEmployeeIdTitle);
	vsTableHead.push_back(CTable::csEmployeeNameTitle);
	vsTableHead.push_back(CTable::csEmployeePositionTitle);
	vector<size_t> viColumnSize;
	viColumnSize.push_back(10);
	viColumnSize.push_back(30);
	viColumnSize.push_back(27);
	RezTable = new CTable(vsTableHead, viColumnSize);

	vector<string> vsIds = CEmployeesContainer::ParseIds(GetActualParameters(argv[2]));
	CEmployee* ServiceElem = m_pEmployeesContainer->GetEmployeeById(vsIds[0]);
	if(ServiceElem == NULL)
	{
		return RezTable;
	}
	vector<CEmployee> vEmployees;
	for(size_t i = 0; i < ServiceElem->GetParents().size(); ++i)
	{
		string sParentServiceId = ServiceElem->GetParents()[i];
		CEmployee* ParentServiceElem = m_pEmployeesContainer->GetEmployeeById(sParentServiceId);
		if(ParentServiceElem == NULL)
		{
			sErrString += "Service has parent with id = " + sParentServiceId + ", but such does not exist!";
			delete RezTable;
			RezTable = NULL;
			return RezTable;
		}
		vEmployees.push_back(*ParentServiceElem);
	}
	RezTable->CreateEmployeesTable(vEmployees);
	return RezTable;
}

CTable* CController::GetColleagues(int argc, char* argv[],string& sErrString)
{
	CTable* RezTable = NULL;
	sErrString.clear();

	vector<string> vsTableHead;
	vsTableHead.push_back(CTable::csEmployeeIdTitle);
	vsTableHead.push_back(CTable::csEmployeeNameTitle);
	vsTableHead.push_back(CTable::csEmployeePositionTitle);
	vector<size_t> viColumnSize;
	viColumnSize.push_back(10);
	viColumnSize.push_back(30);
	viColumnSize.push_back(27);
	RezTable = new CTable(vsTableHead, viColumnSize);

	vector<string> vsIds = CEmployeesContainer::ParseIds(GetActualParameters(argv[2]));
	CEmployee* ServiceElem = m_pEmployeesContainer->GetEmployeeById(vsIds[0]);
	if(ServiceElem == NULL)
	{
		return RezTable;
	}
	vector<string> vsParentsIds;
	for(size_t i = 0; i < ServiceElem->GetParents().size(); ++i)
	{
		vsParentsIds.push_back(ServiceElem->GetParents()[i]);
	}
	set<string> stsChildrenIds;
	vector<CEmployee> vSiblings;
	for(size_t i = 0; i < vsParentsIds.size(); ++i)
	{
		CEmployee* CurrentParent = m_pEmployeesContainer->GetEmployeeById(vsParentsIds[i]);
		if(CurrentParent == NULL)
		{
			sErrString += "Service has parent with id = " + vsParentsIds[i] + ", but such does not exist!";
			delete RezTable;
			RezTable = NULL;
			return RezTable;
		}

		for(size_t i = 0; i < CurrentParent->GetChildren().size(); ++i)
		{
			CEmployee* CurrentChild = m_pEmployeesContainer->GetEmployeeById(CurrentParent->GetChildren()[i]);
			if(CurrentChild == NULL)
			{
				sErrString += "Service has child with id = " + CurrentParent->GetChildren()[i] + ", but such does not exist!";
				delete RezTable;
				RezTable = NULL;
				return RezTable;
			}
			if(stsChildrenIds.find(CurrentChild->GetId()) == stsChildrenIds.end() && CurrentChild->GetId() != ServiceElem->GetId())
			{
				stsChildrenIds.insert(CurrentChild->GetId());
				vSiblings.push_back(*CurrentChild);
			}
		}
	}
	RezTable->CreateEmployeesTable(vSiblings);
	return RezTable;
}

CTable* CController::GetEmployees(int argc, char* argv[], string& sErrString)
{
	CTable* RezTable = NULL;
	sErrString.clear();

	vector<string> vsTableHead;
	vsTableHead.push_back(CTable::csEmployeeIdTitle);
	vsTableHead.push_back(CTable::csEmployeeNameTitle);
	vsTableHead.push_back(CTable::csEmployeePositionTitle);
	vector<size_t> viColumnSize;
	viColumnSize.push_back(10);
	viColumnSize.push_back(30);
	viColumnSize.push_back(27);
	if(argc == 2)
	{
		RezTable = new CTable(vsTableHead, viColumnSize);
		vector<CEmployee> vEmployees;
		for(size_t i = 0; i < m_pEmployeesContainer->GetAllEmployees().size(); ++i)
		{
			vEmployees.push_back(m_pEmployeesContainer->GetAllEmployees()[i]);
		}
		RezTable->CreateEmployeesTable(vEmployees);
		return RezTable;
	}
	else
	{
		string sParams = argv[2];
		string sLeftParam, sRightParam;
		if(sParams.find("&") != string::npos)
		{
			sLeftParam = sParams.substr(0, sParams.find("&"));
			sRightParam = sParams.substr(sParams.find("&") + 1, sParams.size());
			string sIds = CController::GetActualParameters(sLeftParam);
			vector<string> vsIds = CEmployeesContainer::ParseIds(sIds);
			if(!(CEmployeesContainer::ParseIds(CController::GetActualParameters(sRightParam))[0] == "0" ||
				CEmployeesContainer::ParseIds(CController::GetActualParameters(sRightParam))[0] == "1"))
			{
				if(RezTable != NULL)
				{
					delete RezTable;
					RezTable = NULL;
				}
				return RezTable;
			}
			
			bool bRelations = (CEmployeesContainer::ParseIds(CController::GetActualParameters(sRightParam))[0] == "1");

			vector<CEmployee> vEmployees;
			CEmployee* ServiceElem = NULL;
			for(size_t i = 0; i < vsIds.size(); ++i)
			{
				ServiceElem = m_pEmployeesContainer->GetEmployeeById(vsIds[i]);
				if(ServiceElem != NULL)
				{
					vEmployees.push_back(*ServiceElem);
				}
				else
				{
					continue;
				}
			}
			if(bRelations)
			{
				CGraphBuilder* GraphBuilder = new CGraphBuilder(vEmployees, m_pEmployeesContainer);
				RezTable = new CTable(GraphBuilder->GetStructuredTable());
				delete GraphBuilder;
				RezTable->SetIsRelationsTable(true);
				return RezTable;
			}
			else
			{
				RezTable = new CTable(vsTableHead, viColumnSize);
				RezTable->CreateEmployeesTable(vEmployees);
				return RezTable;
			}
		}
		else
		{
			if(sParams.find("employees_id=") != string::npos)
			{
				string sIds = CController::GetActualParameters(sParams);
				vector<string> vsIds = CEmployeesContainer::ParseIds(sIds);
				vector<CEmployee> vEmployees;
				CEmployee* ServiceElem = NULL;
				for(size_t i = 0; i < vsIds.size(); ++i)
				{
					ServiceElem = m_pEmployeesContainer->GetEmployeeById(vsIds[i]);
					if(ServiceElem != NULL)
					{
						vEmployees.push_back(*ServiceElem);
					}
					else
					{
						continue;
					}
				}
				RezTable = new CTable(vsTableHead, viColumnSize);
				RezTable->CreateEmployeesTable(vEmployees);
				return RezTable;
			}
			else
			{
				if(sParams.find("show_relations=") != string::npos)
				{
					bool bRelations = (CEmployeesContainer::ParseIds(CController::GetActualParameters(sParams))[0] == "1");
					vector<CEmployee> vEmployees;
					for(size_t i = 0; i < m_pEmployeesContainer->GetAllEmployees().size(); ++i)
					{
						vEmployees.push_back(m_pEmployeesContainer->GetAllEmployees()[i]);
					}
					if(bRelations)
					{
						CGraphBuilder* GraphBuilder = new CGraphBuilder(vEmployees, m_pEmployeesContainer);
						RezTable = new CTable(GraphBuilder->GetStructuredTable());
						delete GraphBuilder;
						RezTable->SetIsRelationsTable(true);
						return RezTable;
					}
					else
					{
						RezTable = new CTable(vsTableHead, viColumnSize);
						RezTable->CreateEmployeesTable(vEmployees);
						return RezTable;
					}
				}
			}
		}
	}
	return RezTable;
}

CTable* CController::GetEmployeesRelations(int argc, char* argv[], string& sErrString)
{
	CTable* RezTable = NULL;
	sErrString.clear();

	vector<string> vsTableHead;
	vsTableHead.push_back(CTable::csBossIdTitle);
	vsTableHead.push_back(CTable::csBossNameTitle);
	vsTableHead.push_back(CTable::csInferiorIdTitle);
	vsTableHead.push_back(CTable::csInferiorNameTitle);
	vector<size_t> viColumnSize;
	viColumnSize.push_back(10);
	viColumnSize.push_back(23);
	viColumnSize.push_back(10);
	viColumnSize.push_back(23);
	RezTable = new CTable(vsTableHead, viColumnSize);

	vector<string> vsRow;
	if(argc == 2)
	{
		for(size_t i = 0; i < m_pEmployeesContainer->GetAllEmployees().size(); ++i)
		{
			for(size_t j = 0; j < m_pEmployeesContainer->GetAllEmployees()[i].GetChildren().size(); ++j)
			{
				vsRow.push_back(m_pEmployeesContainer->GetAllEmployees()[i].GetId());
				string sFullName = m_pEmployeesContainer->GetAllEmployees()[i].GetSurname() + ' ' +
					m_pEmployeesContainer->GetAllEmployees()[i].GetFirstName() + ' ' +
					m_pEmployeesContainer->GetAllEmployees()[i].GetSecondName();
				vsRow.push_back(sFullName);
				CEmployee* Child = m_pEmployeesContainer->GetEmployeeById(m_pEmployeesContainer->GetAllEmployees()[i].GetChildren()[j]);
				if(Child == NULL)
				{
					continue;
				}
				vsRow.push_back(Child->GetId());
				sFullName = Child->GetSurname() + ' ' + Child->GetFirstName() + ' ' + Child->GetSecondName();
				vsRow.push_back(sFullName);
				RezTable->AddRow(vsRow);
				vsRow.clear();
			}
		}
	}
	else
	{
		string sParams = argv[2];
		bool bParentExist = false;
		bool bChildExist = false;
		size_t iParentPos = sParams.find("boss_id=");
		size_t iChildPos = sParams.find("inferior_id");
		if(iParentPos != string::npos)
		{
			bParentExist = true;
		}
		if(iChildPos != string::npos)
		{
			bChildExist = true;
		}
		string sChildId, sParentId;

		if(bParentExist & bChildExist)
		{
			string sLeft = sParams.substr(0, sParams.find("&"));
			string sRight = sParams.substr(sParams.find("&") + 1, sParams.size());
			sParentId = CEmployeesContainer::ParseIds(GetActualParameters(sLeft))[0];
			sChildId = CEmployeesContainer::ParseIds(GetActualParameters(sRight))[0];
			CEmployee* Parent = m_pEmployeesContainer->GetEmployeeById(sParentId);
			CEmployee* Child = m_pEmployeesContainer->GetEmployeeById(sChildId);
			if(Child == NULL)
			{
				return RezTable;
			}

			if(!Parent->HasChild(sChildId))
			{
				sErrString += "Service with id = " + sParentId + " does not have child with id = " + sChildId;
				if(RezTable != NULL)
				{
					delete RezTable;
					RezTable = NULL;
				}
				return RezTable;
			}
			vsRow.push_back(Parent->GetId());
			string sFullName = Parent->GetSurname() + ' ' + Parent->GetFirstName() + ' ' + Parent->GetSecondName();
			vsRow.push_back(sFullName);
			vsRow.push_back(Child->GetId());
			sFullName = Child->GetSurname() + ' ' + Child->GetFirstName() + ' ' + Child->GetSecondName();
			vsRow.push_back(sFullName);
			RezTable->AddRow(vsRow);
			vsRow.clear();
		}
		else
			if(bParentExist)
			{
				string sParentId = CEmployeesContainer::ParseIds(GetActualParameters(sParams))[0];
				CEmployee* Parent = m_pEmployeesContainer->GetEmployeeById(sParentId);
				if(Parent == NULL)
				{
					return RezTable;
				}
				for(size_t i = 0; i < Parent->GetChildren().size(); ++i)
				{
					CEmployee* Child = m_pEmployeesContainer->GetEmployeeById(Parent->GetChildren()[i]);
					vsRow.push_back(Parent->GetId());
					string sFullName = Parent->GetSurname() + ' ' + Parent->GetFirstName() + ' ' + Parent->GetSecondName();
					vsRow.push_back(sFullName);
					vsRow.push_back(Child->GetId());
					sFullName = Child->GetSurname() + ' ' + Child->GetFirstName() + ' ' + Child->GetSecondName();
					vsRow.push_back(sFullName);
					RezTable->AddRow(vsRow);
					vsRow.clear();
				}
			}
			else
				if(bChildExist)
				{
					string sChildId = CEmployeesContainer::ParseIds(GetActualParameters(sParams))[0];
					CEmployee* Child = m_pEmployeesContainer->GetEmployeeById(sChildId);
					if(Child == NULL)
					{
						return RezTable;
					}
					for(size_t i = 0; i < Child->GetParents().size(); ++i)
					{
						CEmployee* Parent = m_pEmployeesContainer->GetEmployeeById(Child->GetParents()[i]);
						vsRow.push_back(Parent->GetId());
						string sFullName = Parent->GetSurname() + ' ' + Parent->GetFirstName() + ' ' + Parent->GetSecondName();
						vsRow.push_back(sFullName);
						vsRow.push_back(Child->GetId());
						sFullName = Child->GetSurname() + ' ' + Child->GetFirstName() + ' ' + Child->GetSecondName();
						vsRow.push_back(sFullName);
						RezTable->AddRow(vsRow);
						vsRow.clear();
					}
				}
		}
	return RezTable;
}


string CController::GetActualParameters(const string &csData)
{
	size_t iPos = csData.rfind("=");
	if(iPos == string::npos)
	{
		return string("");
	}
	else
	{
		return csData.substr(iPos + 1, csData.size());
	}
}

CTable* CController::ValidateConfiguration(string& sErrString)
{
	sErrString.clear();

	if(m_pEmployeesContainer->IsOK())
	{
		sErrString += "xml-file is OK!";
	}
	else
	{
		sErrString += "Errors found in xml-file!\n";
		size_t iPos = 1; //Every error message starts with '\n'
		while((iPos < m_pEmployeesContainer->GetErrorMsg().size()) && (m_pEmployeesContainer->GetErrorMsg().c_str()[iPos]!='\n'))
		{
			sErrString += m_pEmployeesContainer->GetErrorMsg().c_str()[iPos];
			++iPos;
		}
	}
	return NULL;
}

bool CController::ValidateConfiguration(bool bPrintOkMsg)
{
	if(m_pEmployeesContainer->IsOK())
	{
		if(bPrintOkMsg)
		{
			printf("xml-file is OK!");
		}
		return true;
	}
	else
	{
		printf("Errors found in xml-file!\n");
		size_t iPos = 1; //Every error message starts with '\n'
		while((iPos < m_pEmployeesContainer->GetErrorMsg().size()) && (m_pEmployeesContainer->GetErrorMsg().c_str()[iPos]!='\n'))
		{
			printf("%c",m_pEmployeesContainer->GetErrorMsg().c_str()[iPos]);
			++iPos;
		}
		printf("\n");
		return false;
	}
}

CTable* CController::Execute(int argc, char* argv[], string& sErrString)
{
	if(!ValidateConfiguration(false))
	{
		return ValidateConfiguration(sErrString);
	}

	if(argv[1] == CCommands::csCmdGetEmployeesData)
	{
		return GetEmployeeData(argc, argv, sErrString);
	}
	else
		if(argv[1] == CCommands::csCmdGetInferiors)
		{
			return GetInferiors(argc, argv, sErrString);
		}
		else
			if(argv[1] == CCommands::csCmdGetBoss)
			{
				return GetBosses(argc, argv, sErrString);
			}
			else
				if(argv[1] == CCommands::csCmdGetColleagues)
				{
					return GetColleagues(argc, argv, sErrString);
				}
				else
					if(argv[1] == CCommands::csCmdValidateConfiguration)
					{
						return ValidateConfiguration(sErrString);
					}
					else
						if(argv[1] == CCommands::csCmdGetEmployees)
						{
							return GetEmployees(argc, argv, sErrString);
						}
						else
							if(argv[1] == CCommands::csCmdGetEmployeesRelations)
							{
								return GetEmployeesRelations(argc, argv, sErrString);
							}
	return NULL;
}
	



