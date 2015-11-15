#pragma once
#include "Employee.h"
#include "EmployeesContainer.h"
#include "../shared/Table.h"
#include <map>
#include <set>
#include <vector>
#include <string>
using std::vector;
using std::string;
using std::set;
using std::map;
using std::pair;

class CGraphBuilder
{
public:
	CGraphBuilder(const vector<CEmployee> &vEmployees, CEmployeesContainer* pEmployeesContainer);
	~CGraphBuilder(void);
	CTable GetStructuredTable();
	bool CycleExist(vector<string> &vsCycle);
	void Init();
	void CreateGraphModel();

private:
	size_t GetId(const string &csId);
	void TopologicalSortDFS(size_t v);
	void TopologicalSort();
	void BuildDFS(size_t v, int &iLevel, bool &bFlag);
	void Build();
	bool CycleDFS(int v, vector<char> &vcColor, vector<int> &viP, int &iBegin, int &iEnd);
	CGraphBuilder& operator=(const CGraphBuilder);

public:
	
private:
	const vector<CEmployee> m_vEmployees;
	int m_iCurrentId;
	map<string, size_t> m_mpsiStringId;
	vector<string> m_vsIdString;
	vector< vector<size_t> > m_vviGraphModel;
	vector<bool> m_vbUsed;
	vector<bool> m_vbGlobalUsed;
	vector<size_t> m_viOrder;
	vector< pair<int, string> > m_vpisResult;
	CEmployeesContainer* m_pEmployeesContainer;
};

