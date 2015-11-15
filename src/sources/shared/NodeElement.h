#pragma once
#include <string>
#include <vector>
#include <map>
#include <utility>
using std::string;
using std::vector;
using std::map;

class CNodeElement
{
public:
	CNodeElement(const string& csName);
	~CNodeElement(void);
	void AddAttribute(const string& csAttributeName, const string& csAttributeValue) { m_mpAttributes[csAttributeName] = csAttributeValue; }
	void SetParent(CNodeElement* cpParent) { m_pParent = cpParent; }
	void AddChild(CNodeElement* cpChild) { m_vpChildren.push_back(cpChild); }
	void AppendText(char csTextPart) { m_sText += csTextPart; }
	string GetAttributeValue(const string& csAttributeName) const; 
	string GetName() const { return m_sName; }
	map<string,string> GetAttributes() const { return m_mpAttributes;}
	string GetText() const { return m_sText; }
	CNodeElement* const GetParent() const { return m_pParent; }
	size_t GetNumberOfChildren() const { return m_vpChildren.size(); }
	CNodeElement* const GetChild(size_t iIndex) const;
	static void PrintStack(const CNodeElement* pElem, int& iLevel);
	bool HasChildWithName(const string& csName, int& iFoundIndex) const;

private:

public:
	static const string csNoSuchData;

private:
	string m_sName;
	map<string,string> m_mpAttributes;
	string m_sText;
	CNodeElement* m_pParent;
	vector<CNodeElement*> m_vpChildren;

};

