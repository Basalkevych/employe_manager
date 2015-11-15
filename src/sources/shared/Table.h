#pragma once
#include "../shared/Employee.h"
#include <cassert>
#include "ConsoleColor.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

class CTable
{
public:
	CTable(const vector<string> &cvsTitles, const vector<size_t> &cviMaxColumnWidth);
	~CTable();

	void AddRow(const vector<string> &cvsRow);
	void Print() const;
	void Clear() { m_vvsData.clear(); }
	void CreateDataTable(const vector<CEmployee> &vEmployees);
	void CreateEmployeesTable(const vector<CEmployee> &vEmployees);
	void SetIsRelationsTable(bool f) { m_bIsRelationsTable = f; }
	static string ToXmlString(const CTable& Table);
	static CTable CreateFromXmlString(const string& XmlString);

private:
	void PrintRow(size_t iRowIndex, bool bIsRelationTable) const;
	void PrintElement(string csToPrint, size_t iColumnIndex, vector<size_t> &viAlreadyPrinted, bool bCut = false) const;
	void PrintHead() const;
	void PrintBorder() const;
	void UpdateRealColumnWidth();
	size_t RowTextSize(size_t iRowIndex, bool isRelationTable) const;
	CTable& operator=(const CTable&);

public:
	static const string csEmployeeIdTitle;
	static const string csEmployeeNameTitle;
	static const string csEmployeeSurnameTitle;
	static const string csEmployeeFirstNameTitle;
	static const string csEmployeeSecondNameTitle;
	static const string csEmployeePositionTitle;
	static const string csRoomPlaceProperty;
	static const string csPropertyNameTitle;
	static const string csPropertyValueTitle;
	static const string csBirthdateProperty;
	static const string csProjectProperty;
	static const string csEnglishLevelProperty;
	static const string csPhoneProperty;
	static const string csEmailProperty;
	static const string csSkype;
	static const string csBossIdTitle;
	static const string csBossNameTitle;
	static const string csInferiorIdTitle;
	static const string csInferiorNameTitle;

private:
	vector< vector<string> > m_vvsData;
	const vector<size_t> m_cviMaxColumnWidth;
	vector<size_t> m_viRealColumnWidth;
	vector<size_t> m_viIdents;
	size_t m_iNumberOfColumns;
	size_t m_iTableSize;
	bool m_bIsRelationsTable;

};

