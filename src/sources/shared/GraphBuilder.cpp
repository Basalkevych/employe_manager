#include "GraphBuilder.h"


CGraphBuilder::CGraphBuilder(const vector<CEmployee> &vEmployees, CEmployeesContainer* pEmployeesContainer)
	: m_vEmployees(vEmployees), m_pEmployeesContainer(pEmployeesContainer)
{
	m_iCurrentId = -1;
}

CGraphBuilder::~CGraphBuilder(void)
{
}

bool CGraphBuilder::CycleExist(vector<string> &vsCycle)
{
	vsCycle.clear();
	vector<int> viP(m_vviGraphModel.size(), -1);
	vector<char> vcColor(m_vviGraphModel.size(), 0);
	int iBegin = -1;
	int iEnd = -1;
	for (size_t i = 0; i < m_vviGraphModel.size(); ++i)
	{
		if (CycleDFS(static_cast<int>(i), vcColor, viP, iBegin, iEnd))
		{
			break;
		}
	}

	if(iBegin == -1)
	{
		return false;
	}
	vsCycle.push_back(m_vsIdString[iBegin]);
	for (int v = iEnd; v != iBegin; v = viP[v])
	{
		vsCycle.push_back (m_vsIdString[v]);
	}
	vsCycle.push_back (m_vsIdString[iBegin]);
	std::reverse(vsCycle.begin(), vsCycle.end());
	return true;
}

bool CGraphBuilder::CycleDFS(int v, vector<char> &vcColor, vector<int> &viP, int &iBegin, int &iEnd)
{
	vcColor[v] = 1;
	for (size_t i = 0; i < m_vviGraphModel[v].size(); ++i)
	{
		int to = static_cast<int>(m_vviGraphModel[v][i]);
		if (vcColor[to] == 0) 
		{
			viP[to] = v;
			if (CycleDFS(to, vcColor, viP, iBegin, iEnd))
			{
					return true;
			}
		}
		else 
			if (vcColor[to] == 1) 
			{
				iEnd = v;
				iBegin = to;
			return true;
			}
	}
	vcColor[v] = 2;
	return false;
}

size_t CGraphBuilder::GetId(const string &csId)
{
	if(m_mpsiStringId.find(csId) == m_mpsiStringId.end())
	{
		m_mpsiStringId[csId] = ++m_iCurrentId;
		m_vsIdString.push_back(csId);
		return m_iCurrentId;
	}
	else
	{
		return m_mpsiStringId[csId];
	}
}

void CGraphBuilder::Init()
{
	set<string> stIds;
	for(size_t i = 0; i < m_vEmployees.size(); ++i)
	{
		stIds.insert(m_vEmployees[i].GetId());
	}
	for(auto it = stIds.begin(); it != stIds.end(); ++it)
	{
		GetId(*it);
	}
}

void CGraphBuilder::CreateGraphModel()
{
	m_vviGraphModel.resize(m_iCurrentId + 1);
	for(size_t i = 0; i < m_vEmployees.size(); ++i)
	{
		vector<string> vsMyChildren = m_vEmployees[i].GetChildren();
		for(size_t j = 0; j < m_vEmployees.size(); ++j)
		{
			string sMyId = m_vEmployees[j].GetId();
			for(size_t k = 0; k < vsMyChildren.size(); ++k)
			{
				if(vsMyChildren[k] == sMyId)
				{
					m_vviGraphModel[GetId(m_vEmployees[i].GetId())].push_back(GetId(sMyId));
					break;
				}
			}
		}
	}
}

void CGraphBuilder::TopologicalSortDFS(size_t v)
{
	m_vbUsed[v] = true;
	for (size_t i = 0; i < m_vviGraphModel[v].size(); ++i)
	{
		if(!m_vbUsed[m_vviGraphModel[v][i]])
		{
			TopologicalSortDFS(m_vviGraphModel[v][i]);
		}
	}
	m_viOrder.push_back(v);
}

void CGraphBuilder::TopologicalSort()
{
	m_vbUsed.assign(m_iCurrentId + 1, false);
	for(size_t i = 0; i < m_vbUsed.size(); ++i)
	{
		if(!m_vbUsed[i])
		{
			TopologicalSortDFS(i);
		}
	}
	std::reverse(m_viOrder.begin(), m_viOrder.end());
}

void CGraphBuilder::BuildDFS(size_t v, int &iLevel, bool &bFlag)
{
	if(!m_vbUsed[v])
	{
		bFlag = true;
		m_vpisResult.push_back(std::make_pair(iLevel, m_vsIdString[v]));
		m_vbUsed[v] = true;
		m_vbGlobalUsed[v] = true;
		for(size_t i = 0; i < m_vviGraphModel[v].size(); ++i)
		{
			BuildDFS(m_vviGraphModel[v][i], ++iLevel, bFlag);
		}
	}
	else
	{
		if(bFlag)
		{
			m_vpisResult.push_back(std::make_pair(iLevel, m_vsIdString[v]));
		}
		for(size_t i = 0; i < m_vviGraphModel[v].size(); ++i)
		{
			BuildDFS(m_vviGraphModel[v][i], ++iLevel, bFlag);
		}
	}
	bFlag = false;
	--iLevel;
}

void CGraphBuilder::Build()
{
	m_vbUsed.assign(m_iCurrentId + 1, false);
	m_vbGlobalUsed = m_vbUsed;
	for(size_t i = 0; i < m_viOrder.size(); ++i)
	{
		if(!m_vbGlobalUsed[m_viOrder[i]])
		{
			int iLevel = 0;
			bool bFlag = true;
			m_vbUsed.assign(m_iCurrentId + 1, false);
			BuildDFS(m_viOrder[i], iLevel, bFlag);
		}
	}
}

CTable CGraphBuilder::GetStructuredTable()
{
	vector<string> vsTableHead;
	vsTableHead.push_back(CTable::csEmployeeIdTitle);
	vsTableHead.push_back(CTable::csEmployeeNameTitle);
	vsTableHead.push_back(CTable::csEmployeePositionTitle);
	vector<size_t> viColumnSize;
	viColumnSize.push_back(10);
	viColumnSize.push_back(35);
	viColumnSize.push_back(22);

	CTable Table(vsTableHead, viColumnSize);

	Init();
	CreateGraphModel();
	TopologicalSort();
	Build();
	vector<string> vsRow;
	for(size_t i = 0; i < m_vpisResult.size(); ++i)
	{
		vsRow.clear();
		CEmployee* CurrentEmployee = m_pEmployeesContainer->GetEmployeeById(m_vpisResult[i].second);
		vsRow.push_back(CurrentEmployee->GetId());
		string sName = "";
		if(m_vpisResult[i].first > 0)
		{
			for(int j = 0; j < m_vpisResult[i].first; ++j)
			{
				sName += "  ";
			}
			sName += "|-";
		}
		sName += CurrentEmployee->GetSurname() + ' ' + CurrentEmployee->GetFirstName() +
			' ' + CurrentEmployee->GetSecondName();
		vsRow.push_back(sName);
		vsRow.push_back(CurrentEmployee->GetPosition());

		Table.AddRow(vsRow);
	}
	return Table;
}




