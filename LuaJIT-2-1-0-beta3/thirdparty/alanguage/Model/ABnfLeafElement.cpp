
#include "ABnfLeafElement.h"
#include "../Index/ABnfFile.h"

ABnfLeafElement::ABnfLeafElement(ABnfFactory* factory, ABnfFile* file, int line, int col, int offset, const std::string& value)
    : ABnfElement(factory, file, line, col, offset)
{
    m_value = value;
}

// ����ƫ��λ�ã���ȡ������Ԫ��
ABnfElementPtr ABnfLeafElement::GetException(int offset)
{
    if (offset < GetStart()) return nullptr;
    if (offset >= GetEnd()) return nullptr;
    return shared_from_this();
}

void ABnfLeafElement::CalcEnd()
{
    m_end_line = m_line;
    m_end_col = m_col;

    for (size_t i = 0; i < m_value.size(); ++i)
    {
        char value = m_file->GetText()[m_start + i];
        if (value == '\n')
        {
            m_end_col = 1;
            ++m_end_line;
        }
        else
        {
            ++m_end_col;
        }
    }
}
