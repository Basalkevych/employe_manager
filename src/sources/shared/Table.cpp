#include "Table.h"
#include "../shared/EmployeesContainer.h"
#include "../shared/XmlKeyWords.h"


const string CTable::csEmployeeIdTitle = "ID";
const string CTable::csEmployeeNameTitle = "NAME";
const string CTable::csEmployeeSurnameTitle = "SURNAME";
const string CTable::csEmployeeFirstNameTitle = "FIRSTNAME";
const string CTable::csEmployeeSecondNameTitle = "SECONDNAME";
const string CTable::csEmployeePositionTitle = "POSITION";
const string CTable::csRoomPlaceProperty = "Room/Place";
const string CTable::csPropertyNameTitle = "PROPERTY_NAME";
const string CTable::csPropertyValueTitle = "PROPERTY_VALUE";
const string CTable::csBirthdateProperty = "Birthdate";
const string CTable::csProjectProperty = "Project";
const string CTable::csEnglishLevelProperty = "English";
const string CTable::csPhoneProperty = "Phone";
const string CTable::csEmailProperty = "Email";
const string CTable::csSkype = "Skype";
const string CTable::csBossIdTitle = "BOSS_ID";
const string CTable::csBossNameTitle = "BOSS_NAME";
const string CTable::csInferiorIdTitle = "INFERIOR_ID";
const string CTable::csInferiorNameTitle = "INFERIOR_NAME";


CTable::CTable(const vector<string> &cvsTitles, const vector<size_t> &cviMaxColumnWidth) : m_cviMaxColumnWidth(cviMaxColumnWidth), m_bIsRelationsTable(false)
{
	assert(cvsTitles.size() == cviMaxColumnWidth.size());
	m_iNumberOfColumns = cvsTitles.size();
	m_viRealColumnWidth.assign(m_iNumberOfColumns, 0);
	m_viIdents.assign(m_iNumberOfColumns, 0);
	AddRow(cvsTitles);
}


CTable::~CTable()
{
}

void CTable::AddRow(const vector<string> &vsRow)
{
	assert(vsRow.size() == m_iNumberOfColumns);
	vector<string> vsCurrentRow(m_iNumberOfColumns);
	for(size_t i = 0; i < m_iNumberOfColumns; ++i)
	{
		vsCurrentRow[i] = vsRow[i];
	}
	m_vvsData.push_back(vsCurrentRow);
	UpdateRealColumnWidth();
}

void CTable::UpdateRealColumnWidth()
{
	size_t iLastRowIndex = m_vvsData.size() - 1;
	for(size_t i = 0; i < m_vvsData[iLastRowIndex].size(); ++i)
	{
		m_viRealColumnWidth[i] = std::min(m_cviMaxColumnWidth[i], std::max(m_viRealColumnWidth[i], m_vvsData[iLastRowIndex][i].size()));
		if(i == 0)
		{
			m_viIdents[0] = 0;
		}
		else
		{
			m_viIdents[i] = m_viIdents[i-1] + m_viRealColumnWidth[i-1] + 3;
		}
	}
	m_iTableSize = 1 + 3 * (m_viRealColumnWidth.size());
	for(size_t i = 0; i < m_viRealColumnWidth.size(); ++i)
	{
		m_iTableSize += m_viRealColumnWidth[i];
	}
}

void CTable::PrintBorder() const
{
	printf(" ");
	size_t iCurrPos = 0;
	for(size_t i = 0; i < m_iTableSize - 1; ++i)
	{
		if(iCurrPos<m_viIdents.size() && i == m_viIdents[iCurrPos])
		{
			printf("+");
			++iCurrPos;
		}
		else
		{
			printf("-");
		}
	}
	printf("+\n");
}

void CTable::PrintHead() const
{
	PrintBorder();
	PrintRow(0, false);
	PrintBorder();
}

void CTable::PrintElement(string csToPrint, size_t iColumnIndex, vector<size_t> &viAlreadyPrinted, bool bCut) const
{
	if(bCut)
	{
		if(csToPrint.size() > m_viRealColumnWidth[iColumnIndex])
		{
			csToPrint = csToPrint.substr(0, m_viRealColumnWidth[iColumnIndex] - 3);
			csToPrint += "...";
		}
	}
	static int iRowCount = -1;
	++iRowCount;
	size_t iPosition = viAlreadyPrinted[iColumnIndex];
	size_t iCount = 0;
	int p = 0;
	while(csToPrint[p] == ' ' || csToPrint[p] == '|' || csToPrint[p] == '-')
	{
		++p;
	}
	printf(" | ");

	if(iPosition != 0)
	{
		for(int i = 0; i < p ;++i)
		{
			printf(" ");
			++iCount;
		}
	}
	if(static_cast<size_t>(iRowCount) < m_iNumberOfColumns)
	{
		ConsoleColor::SetColor(ConsoleColor::Yellow, ConsoleColor::Black);
	}
	else
	{
		ConsoleColor::SetColor(ConsoleColor::LightGreen, ConsoleColor::Black);
	}
	while(iPosition < csToPrint.size() && iCount < m_viRealColumnWidth[iColumnIndex])
	{
		printf("%c", csToPrint[iPosition]);
		++iPosition;
		++iCount;
		++viAlreadyPrinted[iColumnIndex];
	}
	ConsoleColor::SetColor(ConsoleColor::LightGray, ConsoleColor::Black);
	if(iCount != m_viRealColumnWidth[iColumnIndex])
	{
		for(size_t i = 0; i < m_viRealColumnWidth[iColumnIndex] - iCount; ++i)
		{
			printf(" ");
		}
	}
	if(iColumnIndex == m_iNumberOfColumns - 1)
	{
		printf(" |\n");
	}
}

size_t CTable::RowTextSize(size_t iRowIndex, bool bIsRelationTable) const
{
	size_t iResult = 0;
	if(!bIsRelationTable)
	{
		for(size_t i = 0; i < m_iNumberOfColumns; ++i)
		{
			iResult += m_vvsData[iRowIndex][i].size();
		}
	}
	else
	{
		for(size_t i = 0; i < m_iNumberOfColumns; ++i)
		{
			iResult += std::min(m_vvsData[iRowIndex][i].size(), m_viRealColumnWidth[i]);
		}
	}

	return iResult;
}

void CTable::PrintRow(size_t iRowIndex, bool isRelationTable) const
{
	vector<size_t> viAlreadyPrinted(m_iNumberOfColumns, 0);
	size_t iCurrentlyPrintedSum = 0;
	size_t iToPrint = RowTextSize(iRowIndex, isRelationTable);
	while(iCurrentlyPrintedSum != iToPrint)
	{
		for(size_t i = 0; i < m_iNumberOfColumns; ++i)
		{
			PrintElement(m_vvsData[iRowIndex][i], i, viAlreadyPrinted, isRelationTable);
		}
		iCurrentlyPrintedSum = 0;
		for(size_t i = 0; i < m_iNumberOfColumns; ++i)
		{
			iCurrentlyPrintedSum += viAlreadyPrinted[i];
		}
	}
}

void CTable::Print() const
{
	PrintHead();
	for(size_t i = 1; i < m_vvsData.size(); ++i)
	{
		PrintRow(i, m_bIsRelationsTable);
		if(!m_bIsRelationsTable)
		{
			PrintBorder();
		}
	}
	if(m_bIsRelationsTable)
	{
		PrintBorder();
	}

}

void CTable::CreateDataTable(const vector<CEmployee> &cvEmployees)
{
	vector<string> cvsHeadders = m_vvsData[0];
	m_vvsData.clear();
	m_vvsData.push_back(cvsHeadders);

	vector<string> vRow;
	for(size_t i = 0; i < cvEmployees.size(); ++i)
	{
		string sEmployeeName = cvEmployees[i].GetSurname() + ' ' + cvEmployees[i].GetFirstName() +
			' ' + cvEmployees[i].GetSecondName();

		if(cvEmployees[i].GetRoomPlace() != CEmployeesContainer::csEmptyProrerty)
		{
			vRow.push_back(sEmployeeName);
			vRow.push_back(CTable::csRoomPlaceProperty);
			vRow.push_back(cvEmployees[i].GetRoomPlace());
			AddRow(vRow);
			vRow.clear();
		}

		if(cvEmployees[i].GetBirthDate() != CEmployeesContainer::csEmptyProrerty)
		{
			vRow.push_back(sEmployeeName);
			vRow.push_back(CTable::csBirthdateProperty);
			vRow.push_back(cvEmployees[i].GetBirthDate());
			AddRow(vRow);
			vRow.clear();
		}

		if(cvEmployees[i].GetProject() != CEmployeesContainer::csEmptyProrerty)
		{
			vRow.push_back(sEmployeeName);
			vRow.push_back(CTable::csProjectProperty);
			vRow.push_back(cvEmployees[i].GetProject());
			AddRow(vRow);
			vRow.clear();
		}

		if(cvEmployees[i].GetEnglishLevel() != CEmployeesContainer::csEmptyProrerty)
		{
			vRow.push_back(sEmployeeName);
			vRow.push_back(CTable::csEnglishLevelProperty);
			vRow.push_back(cvEmployees[i].GetEnglishLevel());
			AddRow(vRow);
			vRow.clear();
		}

		if(cvEmployees[i].GetPhone() != CEmployeesContainer::csEmptyProrerty)
		{
			vRow.push_back(sEmployeeName);
			vRow.push_back(CTable::csPhoneProperty);
			vRow.push_back(cvEmployees[i].GetPhone());
			AddRow(vRow);
			vRow.clear();
		}

		if(cvEmployees[i].GetEmail() != CEmployeesContainer::csEmptyProrerty)
		{
			vRow.push_back(sEmployeeName);
			vRow.push_back(CTable::csEmailProperty);
			vRow.push_back(cvEmployees[i].GetEmail());
			AddRow(vRow);
			vRow.clear();
		}
		
		if(cvEmployees[i].GetSkype() != CEmployeesContainer::csEmptyProrerty)
		{
			vRow.push_back(sEmployeeName);
			vRow.push_back(CTable::csSkype);
			vRow.push_back(cvEmployees[i].GetSkype());
			AddRow(vRow);
			vRow.clear();
		}
	}
}

void CTable::CreateEmployeesTable(const vector<CEmployee> &vEmployees)
{
	vector<string> cvsHeadders = m_vvsData[0];
	m_vvsData.clear();
	m_vvsData.push_back(cvsHeadders);

	vector<string> vRow;
	for(size_t i = 0; i < vEmployees.size(); ++i)
	{
		vRow.push_back(vEmployees[i].GetId());
		string sFullName = vEmployees[i].GetSurname() + ' ' + vEmployees[i].GetFirstName() +
			' ' + vEmployees[i].GetSecondName();
		vRow.push_back(sFullName);
		vRow.push_back(vEmployees[i].GetPosition());
		AddRow(vRow);
		vRow.clear();
	}
}

string CTable::ToXmlString(const CTable& Table)
{
	string sXmlString;
	sXmlString.reserve(1000);
	sXmlString += '<' + CXmlKeyWords::csRoot + '>';
	sXmlString += '<' + CXmlKeyWords::csTable + ' ';
	sXmlString += CXmlKeyWords::csNrows + "=\"" + std::to_string(Table.m_vvsData.size()) + "\" ";
	sXmlString += CXmlKeyWords::csNCols + "=\"" + std::to_string(Table.m_vvsData[0].size()) + "\" ";
	sXmlString += CXmlKeyWords::csIsRelationsTable + "=\"";
	if(Table.m_bIsRelationsTable)
	{
		sXmlString += std::to_string(1);
	}
	else
	{
		sXmlString += std::to_string(0);
	}
	sXmlString += "\">";
	sXmlString += '<' + CXmlKeyWords::csData + '>';
	for(size_t i = 0; i < Table.m_vvsData.size(); ++i)
	{
		sXmlString += '<' + CXmlKeyWords::csRow + '>';
		for(size_t j = 0; j < Table.m_vvsData[i].size(); ++j)
		{
			sXmlString += '<' + CXmlKeyWords::csElement + '>';
			sXmlString += Table.m_vvsData[i][j];
			sXmlString += "</" + CXmlKeyWords::csElement + '>';
		}
		sXmlString += "</" + CXmlKeyWords::csRow + '>';
	}
	sXmlString += "</" + CXmlKeyWords::csData + '>';
	sXmlString += '<' + CXmlKeyWords::csWidth + '>';
	for(size_t i = 0; i < Table.m_cviMaxColumnWidth.size() - 1; ++i)
	{
		sXmlString += std::to_string(Table.m_cviMaxColumnWidth[i]) + ',';
	}
	sXmlString += std::to_string(Table.m_cviMaxColumnWidth.back());
	sXmlString += "</" + CXmlKeyWords::csWidth + '>';
	sXmlString += "</" + CXmlKeyWords::csTable + '>';
	sXmlString += "</" + CXmlKeyWords::csRoot + '>';
	return sXmlString;
}

CTable CTable::CreateFromXmlString(const string& XmlString)
{
	CXmlModel xmlModel(XmlString, false);
	xmlModel.Create();
	vector<const CNodeElement*> vNodes = xmlModel.Get2LevelNodes();
	const CNodeElement* TableNode = vNodes.front();
	int iIndex;
	TableNode->HasChildWithName(CXmlKeyWords::csWidth, iIndex);
	string sColumnWidth = TableNode->GetChild(iIndex)->GetText();
	vector<string> vsColumnWidth = CEmployeesContainer::ParseIds(sColumnWidth);
	vector<size_t> viColumnWidth(vsColumnWidth.size());
	for(size_t i = 0; i < viColumnWidth.size(); ++i)
	{
		viColumnWidth[i] = atoi(vsColumnWidth[i].c_str());
	}

	size_t nCols = viColumnWidth.size();
	size_t nRows = atoi(TableNode->GetAttributeValue(CXmlKeyWords::csNrows).c_str());
	size_t iIsRelationsTable = atoi(TableNode->GetAttributeValue(CXmlKeyWords::csIsRelationsTable).c_str());
	vector< vector<string> > vvsTableData;
	vvsTableData.assign(nRows, vector<string>(nCols));
	TableNode->HasChildWithName(CXmlKeyWords::csData, iIndex);
	const CNodeElement* DataNode = TableNode->GetChild(iIndex);
	for(size_t i = 0; i < nRows; ++i)
	{
		const CNodeElement* Row = DataNode->GetChild(i);
		for(size_t j = 0; j < nCols; ++j)
		{
			vvsTableData[i][j] = Row->GetChild(j)->GetText();
		}
	}
	CTable RezTable(vvsTableData[0], viColumnWidth);
	RezTable.m_bIsRelationsTable = (iIsRelationsTable == 0) ? false : true;
	for(size_t i = 1; i < vvsTableData.size(); ++i)
	{
		RezTable.AddRow(vvsTableData[i]);
	}
	return RezTable;
}
