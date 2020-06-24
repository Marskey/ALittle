
#include "ABnfElement.h"
#include "ABnfFactory.h"
#include "../Index/ABnfFile.h"
#include "../Index/ABnfProject.h"

ABnfElement::ABnfElement(ABnfFactory* factory, ABnfFile* file, int line, int col, int offset)
{
    m_factory = factory;
    m_file = file;
    m_line = line;
    m_col = col;
    m_start = offset;
}

ABnfElement::~ABnfElement()
{
    if (m_reference != nullptr)
        delete m_reference;
}

// ��ȡ����
ABnfReference* ABnfElement::GetReference()
{
    if (m_reference != nullptr) return m_reference;

    if (m_factory != nullptr)
        m_reference = m_factory->CreateReference(shared_from_this());
    if (m_reference == nullptr)
        m_reference = new ABnfReferenceTemplate<ABnfElement>(this);
    return m_reference;
}

// ��ȡ����
void ABnfElement::GuessTypes(std::vector<ABnfGuessPtr>& guess_list, ABnfGuessError& error)
{
    return m_factory->GuessTypes(shared_from_this(), guess_list, error);
}

// ��ȡ��һ������
ABnfGuessPtr ABnfElement::GuessType(ABnfGuessError& error)
{
    std::vector<ABnfGuessPtr> guess_list;
    GuessTypes(guess_list, error);
    if (error.error.size()) return nullptr;
    if (guess_list.size() == 0)
    {
        error.element = shared_from_this();
        error.error = "unknown type";
        return nullptr;
    }
    return guess_list[0];
}

// ��ȡ�ļ�ȫ·��
const std::string& ABnfElement::GetFullPath()
{
    static std::string empty;
    if (m_file == nullptr) return empty;
    return m_file->GetFullPath();
}

// ��ȡ���ڹ���·��
const std::string& ABnfElement::GetProjectPath()
{
    static std::string empty;
    if (m_file == nullptr) return empty;
    auto* project = m_file->GetProjectInfo();
    if (project == nullptr) return empty;
    return project->GetProjectPath();
}

// ���ø��ڵ�

void ABnfElement::SetParent(ABnfNodeElementPtr parent) { m_parent = parent; }

ABnfNodeElementPtr ABnfElement::GetParent() { return m_parent.lock(); }

// ��ǰ�ڵ��Ƿ��ָ����Χ�н���

bool ABnfElement::IntersectsWith(int start, int end)
{
    if (m_start >= end) return false;
    if (GetEnd() <= start) return false;
    return true;
}

// ��ȡ����
const std::string& ABnfElement::GetNodeType() { static std::string empty; return empty; }

// ��ȡ�ı�
const std::string& ABnfElement::GetElementText()
{
    if (m_element_text.size() > 0) return m_element_text;

    int start = GetStart();
    if (start >= m_file->GetLength())
        return m_element_text;

    int length = GetLength();
    if (length == 0) return m_element_text;

    m_element_text = m_file->Substring(start, length);
    return m_element_text;
}

// ��ȡȥ�������ŶԺ�˫���Ŷ�֮����ַ���
std::string ABnfElement::GetElementString()
{
    int length = GetLength();
    if (length <= 2) return "";
    length -= 2;
    int start = GetStart() + 1;
    if (start >= m_file->GetLength()) return "";
    return m_file->Substring(start, length);
}
