#include "ABnf.h"
#include "../Index/ABnfFile.h"
#include "ABnfFactory.h"
#include "ABnfRuleInfo.h"
#include "ABnfRuleNodeInfo.h"
#include "ABnfNodeElement.h"
#include "ABnfKeyElement.h"
#include "ABnfStringElement.h"
#include "ABnfRegexElement.h"
#include "ABnfErrorElement.h"

// ��ʼ������
void ABnf::Clear()
{
    m_file = nullptr;
    m_factory = nullptr;
    m_regex_skip.clear();
    m_line_comment_skip.clear();
    m_block_comment_skip.clear();
    m_root = nullptr;
    m_line_comment = nullptr;
    m_block_comment = nullptr;
    m_rule.Clear();
    m_symbol_check.clear();
}

// �����ķ�
bool ABnf::Load(const std::string& buffer, ABnfFactory* factory, std::string& error)
{
    // ����
    Clear();

    // ���ýڵ㹤��������ָ�벻��Ϊ��
    m_factory = factory;
    if (m_factory == nullptr)
    {
        Clear();
        error = "m_factory == null";
        return false;
    }

    // ���������ķ�
    if (!m_rule.Load(buffer, error))
    {
        Clear();
        return false;
    }

    // ��ȡ������
    m_root = m_rule.FindRuleInfo("Root");
    // ��ȡ����ע��
    m_line_comment = m_rule.FindRuleInfo("LineComment");
    // ��ȡ����ע��
    m_block_comment = m_rule.FindRuleInfo("BlockComment");

    // �ռ����ż�飬�ڴ�������+=ʱ�������Ϊ+=��������+��=��
    const auto& symbol_set = m_rule.GetSymbolSet();
    for (auto& symbol : symbol_set)
    {
        for (auto& symbol_check : symbol_set)
        {
            if (symbol_check.find(symbol) == 0 && symbol_check.size() > symbol.size())
                m_symbol_check[symbol].insert(symbol_check[symbol.size()]);
        }
    }

    return true;
}

// ��ȡ������Ϣ
ABnfRuleInfo* ABnf::GetRule(const std::string& name)
{
    return m_rule.FindRuleInfo(name);
}

// ��ѯ�ؼ���
void ABnf::QueryKeyWordCompletion(const std::string& input, std::vector<ALanguageCompletionInfo>& list)
{
    for (auto& key : m_rule.GetKeySet())
    {
        if (key.find(input) == 0)
        {
            ALanguageCompletionInfo info;
            info.display = key;
            list.push_back(info);
        }
    }
}

// �����﷨�ڵ�
ABnfNodeElementPtr ABnf::CreateNodeElement(int line, int col, int offset, const std::string& type)
{
    auto node = m_factory->CreateNodeElement(m_file, line, col, offset, type);
    if (node == nullptr) node = ABnfNodeElementPtr(new ABnfNodeElement(m_factory, m_file, line, col, offset, type));
    return node;
}

// �����ؼ��ֽڵ�
ABnfKeyElementPtr ABnf::CreateKeyElement(int line, int col, int offset, const std::string& value)
{
    auto node = m_factory->CreateKeyElement(m_file, line, col, offset, value);
    if (node == nullptr) node = ABnfKeyElementPtr(new ABnfKeyElement(m_factory, m_file, line, col, offset, value));
    return node;
}

// �������Žڵ�
ABnfStringElementPtr ABnf::CreateStringElement(int line, int col, int offset, const std::string& value)
{
    auto node = m_factory->CreateStringElement(m_file, line, col, offset, value);
    if (node == nullptr) node = ABnfStringElementPtr(new ABnfStringElement(m_factory, m_file, line, col, offset, value));
    return node;
}

// ����������ʽ�ڵ�
ABnfRegexElementPtr ABnf::CreateRegexElement(int line, int col, int offset, const std::string& value, std::shared_ptr<std::regex> regex)
{
    auto node = m_factory->CreateRegexElement(m_file, line, col, offset, value, regex);
    if (node == nullptr) node = ABnfRegexElementPtr(new ABnfRegexElement(m_factory, m_file, line, col, offset, value, regex));
    return node;
}

// ��ʼ�����ļ�
ABnfNodeElementPtr ABnf::Analysis(ABnfFile* file)
{
    if (m_root == nullptr) return nullptr;

    // ��ջ���
    m_regex_skip.clear();
    m_line_comment_skip.clear();
    m_block_comment_skip.clear();

    // �����ַ���
    m_file = file;

    // ��ʼ��λ��
    int line = 0;
    int col = 0;
    int offset = 0;
    int pin_offset = -1;
    bool not_key = false;
    m_stop_stack.clear();

    // �������ڵ�
    auto node = CreateNodeElement(line, col, offset, m_root->id.value);

    // ��ʼ����
    while (true)
    {
        // ���ݸ��ڵ���ķ���ʼ�����ı�
        if (!AnalysisABnfNode(m_root, m_root->node, node, not_key
            , line, col, offset, pin_offset, false))
        {
            // ���offset���ļ�β������ô��ǰ��һ���ַ����������ڴ�������ڵ�
            if (offset >= m_file->GetLength() && m_file->GetLength() > 0)
                --offset;
        }
        else
        {
            // �����ָ��ַ�
            AnalysisSkip(line, col, offset);

            // ��������ļ�β������ô����������
            if (offset >= m_file->GetLength()) break;
        }

        // ��offset��Ϊ����ڵ�λ��
        node->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, "syntax error", nullptr)));

        // ������һ�У�Ȼ���������
        if (!JumpToNextLine(line, col, offset)) break;
    }

    // ��ջ���
    m_regex_skip.clear();
    m_line_comment_skip.clear();
    m_block_comment_skip.clear();
    m_stop_stack.clear();
    m_file = nullptr;

    // ���ؽ��
    return node;
}

// �����������
bool ABnf::AnalysisABnfNode(ABnfRuleInfo* rule, ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent
    , bool not_key, int& line, int& col, int& offset, int& pin_offset, bool ignore_error)
{
    // ���� �����ҽ���һ����
    if (node->repeat == ABnfRuleNodeRepeatType::NRT_NONE || node->repeat == ABnfRuleNodeRepeatType::NRT_ONE)
    {
        // ƥ���һ��
        int temp_pin_offset = -1;
        if (!AnalysisABnfNodeMatch(rule, node, parent, not_key
            , line, col, offset
            , temp_pin_offset, ignore_error))
        {
            // ���ƥ���ڲ���pin����ôҲҪ������Ϊpin
            if (temp_pin_offset >= 0) pin_offset = temp_pin_offset;

            // ����ƥ��ʧ��
            return false;
        }

        // ���ƥ���ڲ���pin����ôҲҪ������Ϊpin
        if (temp_pin_offset >= 0) pin_offset = temp_pin_offset;

        // ����ƥ��ɹ�
        return true;
    }

    // ���� ������һ����
    if (node->repeat == ABnfRuleNodeRepeatType::NRT_AT_LEAST_ONE)
    {
        // ƥ���һ��
        int temp_pin_offset = -1;
        if (!AnalysisABnfNodeMatch(rule, node, parent, not_key
            , line, col, offset
            , temp_pin_offset, ignore_error))
        {
            // ���ƥ���ڲ���pin����ôҲҪ������Ϊpin
            if (temp_pin_offset >= 0) pin_offset = temp_pin_offset;

            // ����ƥ��ʧ��
            return false;
        }

        // ���ƥ���ڲ���pin����ôҲҪ������Ϊpin
        if (temp_pin_offset >= 0) pin_offset = temp_pin_offset;

        // ƥ��ʣ�µģ����Դ�����ƥ�䵽�����ƥ�䵽����
        return AnalysisABnfNodeMore(rule, node, parent, not_key
            , line, col, offset
            , pin_offset, true);
    }

    // ���� ��û�л���һ����
    if (node->repeat == ABnfRuleNodeRepeatType::NRT_ONE_OR_NOT)
    {
        // ���Դ��������ƥ�䵽��ƥ�䣬���ƥ�䲻��������
        int temp_pin_offset = -1;
        if (!AnalysisABnfNodeMatch(rule, node, parent, not_key
            , line, col, offset
            , temp_pin_offset, true))
        {
            // ���ƥ���ڲ���pin����ôҲҪ������Ϊpin
            // ������Ϊƥ��ʧ��
            if (temp_pin_offset >= 0)
            {
                pin_offset = temp_pin_offset;
                return false;
            }

            // �ڲ�û��pin�����Ա��Ϊ��ǰƥ��ɹ�������ʧ�ܵĲ���
            return true;
        }

        // ���ƥ���ڲ���pin����ôҲҪ������Ϊpin
        if (temp_pin_offset >= 0) pin_offset = temp_pin_offset;

        // ����ƥ��ɹ�
        return true;
    }

    // ���� ��û�л�����������
    if (node->repeat == ABnfRuleNodeRepeatType::NRT_NOT_OR_MORE)
    {
        // ���Դ�����ƥ����پ�ƥ�����
        return AnalysisABnfNodeMore(rule, node, parent, not_key
            , line, col, offset
            , pin_offset, true);
    }

    // ����һ�㲻�ᷢ��
    return false;
}

// ���ƥ��
bool ABnf::AnalysisABnfNodeMore(ABnfRuleInfo* rule, ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent
    , bool not_key, int& line, int& col, int& offset, int& pin_offset, bool ignore_error)
{
    // ѭ������
    while (offset < m_file->GetLength())
    {
        int temp_pin_offset = -1;
        if (!AnalysisABnfNodeMatch(rule, node, parent, not_key
            , line, col, offset
            , temp_pin_offset, ignore_error))
        {
            // ���ƥ���ڲ���pin����ôҲҪ������Ϊpin
            // ������Ϊƥ��ʧ��
            if (temp_pin_offset >= 0)
            {
                // ��������ʹ��offset��Ϊpin
                pin_offset = offset;
                return false;
            }

            // �ڲ�û��pin�����Ա��Ϊ��ǰƥ��ɹ�������ʧ�ܵĲ���
            return true;
        }

        // ���ƥ���ڲ���pin����ôҲҪ������Ϊpin
        if (temp_pin_offset >= 0) pin_offset = temp_pin_offset;

        // ����ע��
        AnalysisABnfCommentMatch(rule, parent, not_key, line, col, offset);
        // �����ָ��ַ�
        AnalysisSkip(line, col, offset);
    }

    return true;
}

// ����ƥ��
bool ABnf::AnalysisABnfRuleMatch(ABnfRuleInfo* rule, ABnfNodeElementPtr parent, bool not_key
    , int& line, int& col, int& offset, int& pin_offset, bool ignore_error)
{
    // �����ָ��ַ�
    AnalysisSkip(line, col, offset);
    // ����ע��
    AnalysisABnfCommentMatch(rule, parent, not_key, line, col, offset);
    // �����ָ��ַ�
    AnalysisSkip(line, col, offset);

    // ����ļ��Ѿ���������ô����ƥ��ʧ��
    if (offset >= m_file->GetLength()) return false;
    // �����һ���ַ�
    char next_char = m_file->GetText()[offset];
    // �����һ���ַ��Ƿ���Ԥ�ⷶΧ��
    auto* index_list = rule->CheckNextChar(next_char);
    if (!index_list) return false;

    // ����ѡ�����
    std::vector<ABnfNodeElementPtr> option_list;
    for (auto option_index : *index_list)
    {
        // ǿ����Ԥ���
        if (!rule->node->PreCheck(m_file->GetText(), offset)) continue;
        // ��ȡ��ǰ�ڵ����
        auto& node_list = rule->node->node_list[option_index];

        // ����λ�ã����ڶ���ƥ��
        int temp_line = line;
        int temp_col = col;
        int temp_offset = offset;

        // ��ǵ�ǰ�����Ƿ���pin
        int temp_pin_offset = -1;
        // �Ƿ�ƥ��ɹ�
        bool match = true;
        // ��ʼ�����������
        ABnfNodeElementPtr element = CreateNodeElement(line, col, offset, rule->id.value);
        for (int index = 0; index < static_cast<int>(node_list.size()); ++index)
        {
            int sub_pin_offset = -1;
            if (!AnalysisABnfNode(rule, node_list[index], element, not_key
                , temp_line, temp_col, temp_offset
                , sub_pin_offset, false))
            {
                // ���ƥ��ʧ�ܣ������ڲ���pin����ô��ǰҲҪ����Ϊpin
                if (sub_pin_offset >= 0) temp_pin_offset = sub_pin_offset;
                match = false;
                break;
            }

            // ���ƥ��ʧ�ܣ������ڲ���pin����ô��ǰҲҪ����Ϊpin
            if (sub_pin_offset >= 0) temp_pin_offset = sub_pin_offset;

            // �����������pin����ôҲҪ����Ϊpin
            if (node_list[index]->pin == ABnfRuleNodePinType::NPT_TRUE)
                temp_pin_offset = temp_offset;
        }

        // ƥ��ɹ�
        if (match)
        {
            // ��ӵ��ڵ���
            if (element->GetChilds().size() != 0) parent->AddChild(element);

            // ���ؽ��λ��
            line = temp_line;
            col = temp_col;
            offset = temp_offset;

            // ���ƥ���ڲ���pin����ôҲҪ������Ϊpin
            if (temp_pin_offset >= 0) pin_offset = temp_pin_offset;

            // ����ƥ��ɹ�
            return true;
        }

        // �������pin����ô����Ƚ�pin
        // ����֮ǰ�Ľڵ㣬����Լ�������
        if (temp_pin_offset >= 0)
        {
            pin_offset = temp_pin_offset;

            line = temp_line;
            col = temp_col;
            offset = temp_offset;

            option_list.clear();
            option_list.push_back(element);
            break;
        }
        // ���û�г���pin���Ѵ�����ӵ�option_list
        else
        {
            option_list.push_back(element);
        }
    }

    // û��pin���Һ��Դ��������£�ֱ�ӷ���false
    if (pin_offset < 0 && ignore_error) return false;

    // ����option_list
    for (auto& option : option_list)
    {
        if (option->GetChilds().size() != 0) parent->AddChild(option);
    }

    // �����pin�������н�����
    if (pin_offset >= 0)
    {
        // ��pin_offset��ʼ���ҽ�����
        int find = m_file->GetLength();
        int index = -1;
        for (int i = static_cast<int>(m_stop_stack.size()) - 1; i >= 0; --i)
        {
            const auto& stop_token = m_stop_stack[i]->GetStopToken();
            if (stop_token.empty()) continue;
            // ���ҽ�����
            size_t value = m_file->GetText().find(stop_token.c_str(), pin_offset, find - pin_offset);
            // ����ҵ��ˣ����ұ�find��������ô�����õ�ǰ���
            if (value != std::string::npos && find > static_cast<int>(value))
            {
                find = static_cast<int>(value);
                index = i;
            }
        }

        // ������ҵ�
        if (index >= 0)
        {
            // ����ǵ�ǰ����
            if (m_stop_stack[index] == rule)
            {
                // �����ָ���
                AnalysisOffset(find + static_cast<int>(m_stop_stack[index]->GetStopToken().size()) - offset, line, col, offset);
                // ���syntax error
                parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, "syntax error", nullptr)));
                // ����ƥ��ɹ���Ŀ�������ɴ����ı�����ʣ�µ��ı�����ƥ��
                return true;
            }
            // ����ǵ����ڶ�������
            else if (index == static_cast<int>(m_stop_stack.size()) - 2)
            {
                // �����ָ���
                AnalysisOffset(find - offset, line, col, offset);
                // ���syntax error
                parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, "syntax error", nullptr)));
                // ����ƥ��ɹ���Ŀ�������ɴ����ı�����ʣ�µ��ı�����ƥ��
                return true;
            }
        }
    }

    return false;
}

// �ڵ�ƥ��
bool ABnf::AnalysisABnfNodeMatch(ABnfRuleInfo* rule, ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
    , int& line, int& col, int& offset, int& pin_offset, bool ignore_error)
{
    // �ж��ǲ���Ҷ�ӽڵ�
    if (node->value.type != ABnfRuleTokenType::TT_NONE)
    {
        // �����ƥ���ӹ���
        if (node->value.type == ABnfRuleTokenType::TT_ID)
        {
            // �����ӹ��򣬲�����
            ABnfRuleInfo* child = node->value.rule;
            if (child == nullptr)
            {
                child = m_rule.FindRuleInfo(node->value.value);
                node->value.rule = child;
            }
            // ����û���ҵ������
            if (child == nullptr)
            {
                // ������Դ���ֱ�ӷ���false
                if (ignore_error) return false;
                // �����ָ���
                AnalysisSkip(line, col, offset);
                // ��Ӵ���ڵ�
                parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, "unknow rule:" + node->value.value, nullptr)));
                // ����ƥ��ʧ��
                return false;
            }

            // ��ӽ�����
            m_stop_stack.push_back(child);

            // ƥ���ӹ����ӹ����pin�ǲ��ܴ�������
            bool result = AnalysisABnfRuleMatch(child, parent, node->not_key == ABnfRuleNodeNotKeyType::NNKT_TRUE || not_key
                , line, col, offset, pin_offset, ignore_error);

            // ɾ��������
            m_stop_stack.pop_back();
            return result;
        }

        // �����������ʽ
        if (node->value.type == ABnfRuleTokenType::TT_REGEX)
            return AnalysisABnfRegexMatch(rule, node, parent, node->not_key == ABnfRuleNodeNotKeyType::NNKT_TRUE || not_key, line, col, offset, pin_offset, ignore_error);

        // ����ǹؼ���
        if (node->value.type == ABnfRuleTokenType::TT_KEY)
            return AnalysisABnfKeyMatch(rule, node, parent, node->not_key == ABnfRuleNodeNotKeyType::NNKT_TRUE || not_key, line, col, offset, ignore_error);

        // ʣ�µľ�����ͨ�ַ���
        return AnalysisABnfStringMatch(rule, node, parent, node->not_key == ABnfRuleNodeNotKeyType::NNKT_TRUE || not_key, line, col, offset, ignore_error);
    }

    // �����һ�������

    // �����ָ���
    AnalysisSkip(line, col, offset);
    // ����ע��
    AnalysisABnfCommentMatch(rule, parent, not_key, line, col, offset);
    // �����ָ���
    AnalysisSkip(line, col, offset);

    // ����Ƿ�end of file
    if (offset >= m_file->GetLength()) return false;
    // �����һ���ַ��Ƿ���Ԥ�ⷶΧ��
    char next_char = m_file->GetText()[offset];
    auto* index_list = node->CheckNextChar(&m_rule, next_char);
    if (!index_list) return false;

    // ����ѡ�����
    std::vector<ABnfNodeElementPtr> option_list;
    for (auto option_index : *index_list)
    {
        // ǿ����Ԥ���
        if (!node->PreCheck(m_file->GetText(), offset)) continue;
        // ��ȡ��ǰ�ڵ����
        auto& node_list = node->node_list[option_index];

        // ����λ�ã����ڶ���ƥ��
        int temp_line = line;
        int temp_col = col;
        int temp_offset = offset;

        // ��ǵ�ǰ�����Ƿ���pin
        int temp_pin_offset = -1;
        // �Ƿ�ƥ��ɹ�
        bool match = true;
        // ��ʼ�������
        ABnfNodeElementPtr element = ABnfNodeElementPtr(new ABnfNodeElement(m_factory, m_file, line, col, offset, ""));
        for (int index = 0; index < static_cast<int>(node_list.size()); ++index)
        {
            int sub_pin_offset = -1;
            if (!AnalysisABnfNode(rule, node_list[index], element, not_key
                , temp_line, temp_col, temp_offset
                , sub_pin_offset, false))
            {
                // ���ƥ��ʧ�ܣ������ڲ���pin����ô��ǰҲҪ����Ϊpin
                if (sub_pin_offset >= 0) temp_pin_offset = sub_pin_offset;
                match = false;
                break;
            }

            // ���ƥ��ʧ�ܣ������ڲ���pin����ô��ǰҲҪ����Ϊpin
            if (sub_pin_offset >= 0) temp_pin_offset = sub_pin_offset;

            // �����������pin����ôҲҪ����Ϊpin
            if (node_list[index]->pin == ABnfRuleNodePinType::NPT_TRUE)
                temp_pin_offset = temp_offset;
        }

        // ƥ��ɹ�
        if (match)
        {
            // ��ӵ��ڵ���
            for (auto& child : element->GetChilds())
            {
                if (child->IsLeafOrHasChildOrError())
                    parent->AddChild(child);
            }
            // ���ؽ��λ��
            line = temp_line;
            col = temp_col;
            offset = temp_offset;

            if (temp_pin_offset >= 0)
                pin_offset = temp_pin_offset;
            return true;
        }

        // �������pin����ô����Ƚ�pin
        // ����֮ǰ�Ľڵ㣬����Լ�������
        if (temp_pin_offset >= 0)
        {
            pin_offset = temp_pin_offset;

            line = temp_line;
            col = temp_col;
            offset = temp_offset;

            option_list.clear();
            option_list.push_back(element);
            break;
        }
        // ���û�г���pin���Ѵ�����ӵ�option_list
        else
        {
            option_list.push_back(element);
        }
    }

    // û��pin���Һ��Դ��������£�ֱ�ӷ���false
    if (pin_offset < 0 && ignore_error) return false;

    // ����option_list
    for (auto& option : option_list)
    {
        for (auto& child : option->GetChilds())
        {
            if (child->IsLeafOrHasChildOrError())
                parent->AddChild(child);
        }
    }

    return false;
}

// ƥ��ؼ���
bool ABnf::AnalysisABnfKeyMatch(ABnfRuleInfo* rule
    , ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
    , int& line, int& col, int& offset, bool ignore_error)
{
    // �����ָ���
    AnalysisSkip(line, col, offset);

    bool succeed = true;
    // ����ַ�����ƥ��
    for (int i = 0; i < static_cast<int>(node->value.value.size()); ++i)
    {
        if (offset + i >= m_file->GetLength()
            || node->value.value[i] != m_file->GetText()[offset + i])
        {
            succeed = false;
            break;
        }
    }

    // ƥ��ɹ�֮�󣬺�����Ǹ��ַ����������֣���ĸ���»���
    // ������Ҫ��ؼ��֣����������֣���ĸ���»��߹���
    if (succeed)
    {
        int next_offset = offset + static_cast<int>(node->value.value.size());
        if (next_offset < m_file->GetLength())
        {
            char next_char = m_file->GetText()[next_offset];
            if (next_char >= '0' && next_char <= '9'
                || next_char >= 'a' && next_char <= 'z'
                || next_char >= 'A' && next_char <= 'Z'
                || next_char == '_')
                succeed = false;
        }
    }

    // ���ƥ��ʧ��
    if (!succeed)
    {
        // �����ע�;�����
        if (rule == m_line_comment || rule == m_block_comment) return false;
        // ������Դ��������
        if (ignore_error) return false;
        // ��Ӵ���ڵ�
        if (offset < m_file->GetLength())
            parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, rule->id.value + "expect" + node->value.value + " but get " + m_file->GetText()[offset]
                , ABnfKeyElementPtr(new ABnfKeyElement(m_factory, m_file, line, col, offset, node->value.value)))));
        else
            parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, rule->id.value + "expect" + node->value.value + " but get end of file"
                , ABnfKeyElementPtr(new ABnfKeyElement(m_factory, m_file, line, col, offset, node->value.value)))));
        return false;
    }

    // �����ȷ�Ľڵ�
    parent->AddChild(CreateKeyElement(line, col, offset, node->value.value));
    // �����ָ���
    AnalysisOffset(static_cast<int>(node->value.value.size()), line, col, offset);
    // ����ƥ��ɹ�
    return true;
}

//��ƥ�����
bool ABnf::AnalysisABnfStringMatch(ABnfRuleInfo* rule
    , ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
    , int& line, int& col, int& offset, bool ignore_error)
{
    // �����ָ���
    AnalysisSkip(line, col, offset);

    bool succeed = true;
    // ����ַ�����ƥ��
    for (int i = 0; i < static_cast<int>(node->value.value.size()); ++i)
    {
        if (offset + i >= m_file->GetLength()
            || node->value.value[i] != m_file->GetText()[offset + i])
        {
            succeed = false;
            break;
        }
    }

    // ƥ��ɹ�֮�󣬽�������ƥ�䣬���з��ż��
    if (succeed)
    {
        int next = offset + static_cast<int>(node->value.value.size());
        if (next < m_file->GetLength())
        {
            auto it = m_symbol_check.find(node->value.value);
            if (it != m_symbol_check.end() && it->second.find(m_file->GetText()[next]) != it->second.end())
                succeed = false;
        }
    }

    // ����ƥ��ʧ��
    if (!succeed)
    {
        // �����ע�;�����
        if (rule == m_line_comment || rule == m_block_comment) return false;
        // ������Դ��������
        if (ignore_error) return false;
        // ��Ӵ���ڵ�
        if (offset < m_file->GetLength())
            parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, rule->id.value + "expect" + node->value.value + " but get " + m_file->GetText()[offset]
                , ABnfStringElementPtr(new ABnfStringElement(m_factory, m_file, line, col, offset, node->value.value)))));
        else
            parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, rule->id.value + "expect" + node->value.value + " but get end of file"
                , ABnfStringElementPtr(new ABnfStringElement(m_factory, m_file, line, col, offset, node->value.value)))));
        return false;
    }

    // �����ȷ�Ľڵ�
    parent->AddChild(CreateStringElement(line, col, offset, node->value.value));
    // �����ָ���
    AnalysisOffset(static_cast<int>(node->value.value.size()), line, col, offset);
    // ����ƥ��ɹ�
    return true;
}

// ƥ��������ʽ
bool ABnf::AnalysisABnfRegexMatch(ABnfRuleInfo* rule
    , ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
    , int& line, int& col, int& offset, int& pin_offset, bool ignore_error)
{
    // �����ָ���
    AnalysisSkip(line, col, offset);

    // ��ȡ���棬���ڱܿ���Ч���ظ�ƥ��
    int length = 0;
    bool cache = false;
    // ���ҵ�ǰƫ��
    auto it = m_regex_skip.find(offset);
    if (it != m_regex_skip.end())
    {
        // ���ҽڵ�ƥ��
        auto length_it = it->second.find(node);
        if (length_it != it->second.end())
        {
            // ���Ϊ�ҵ�
            cache = true;
            length = length_it->second;
        }
    }
    // ���û���ҵ�����ô�ͽ���ƥ��
    if (!cache)
    {
        // ������ʽƥ��
        if (node->value.regex == nullptr)
            node->value.regex = std::shared_ptr<std::regex>(new std::regex(node->value.value));
        // ��ʼƥ��
        std::match_results<const char*> match;
        if (std::regex_search(m_file->GetText().c_str() + offset, match, *node->value.regex, std::regex_constants::match_flag_type::match_continuous) && match.size())
            length = static_cast<int>(match[0].length());
        // ���û��ƥ�䵽�����ҹ����Ԥ��ֵ��pin
        if (length == 0 && rule->prediction.type != ABnfRuleTokenType::TT_NONE && rule->prediction_pin == ABnfRuleNodePinType::NPT_TRUE)
        {
            // ������ʽƥ��
            if (rule->prediction.regex == nullptr)
                rule->prediction.regex = std::shared_ptr<std::regex>(new std::regex(rule->prediction.value));
            // Ԥ��ƥ�䣬���ƥ��ɹ�����ô�ͱ��Ϊ��������ʾ����
            std::match_results<const char*> pre_match;
            if (std::regex_search(m_file->GetText().c_str() + offset, pre_match, *rule->prediction.regex, std::regex_constants::match_continuous) && pre_match.size())
                length = - static_cast<int>(pre_match[0].length());
        }
        // ��ӻ���
        m_regex_skip[offset][node] = length;
    }

    // ������ҵ�����ô�������ȷ�ڵ�
    if (length > 0)
    {
        // ��ȡ�ڵ�����
        std::string value = m_file->Substring(offset, length);
        // ������ʽƥ��Ľ�������ǹؼ���
        if (not_key || m_rule.GetKeySet().find(value) == m_rule.GetKeySet().end())
        {
            // ��ӵ����ڵ�
            parent->AddChild(CreateRegexElement(line, col, offset, value, node->value.regex));
            // �����ָ���
            AnalysisOffset(length, line, col, offset);
            // ����ƥ��ɹ�
            return true;
        }
    }

    // �����ע����ô����Ӵ���ڵ�
    if (rule == m_line_comment || rule == m_block_comment) return false;
    // ������Դ���Ҳ����Ӵ���ڵ�
    if (ignore_error) return false;

    // ��Ӵ���ڵ�
    if (offset < m_file->GetLength())
    {
        // ���length������
        if (length > 0)
        {
            parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset
                , rule->id.value + "expect" + node->value.value + " but get keyword" + m_file->Substring(offset, length)
                , ABnfRegexElementPtr(new ABnfRegexElement(m_factory, m_file, line, col, offset, "", node->value.regex)))));
        }
        else if (length < 0)
        {
            parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, rule->id.value + "expect" + node->value.value + " but get " + m_file->Substring(offset, -length)
                , ABnfRegexElementPtr(new ABnfRegexElement(m_factory, m_file, line, col, offset, "", node->value.regex)))));
            AnalysisOffset(-length, line, col, offset);
            pin_offset = offset - length;
        }
        else
            parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, rule->id.value + "expect" + node->value.value + " but get " + m_file->GetText()[offset]
                , ABnfRegexElementPtr(new ABnfRegexElement(m_factory, m_file, line, col, offset, "", node->value.regex)))));
    }
    else
        parent->AddChild(ABnfErrorElementPtr(new ABnfErrorElement(m_factory, m_file, line, col, offset, rule->id.value + "expect" + node->value.value + " but get end of file"
            , ABnfRegexElementPtr(new ABnfRegexElement(m_factory, m_file, line, col, offset, "", node->value.regex)))));
    return false;
}

// ע��ƥ��
bool ABnf::AnalysisABnfCommentMatch(ABnfRuleInfo* rule, ABnfNodeElementPtr parent, bool not_key, int& line, int& col, int& offset)
{
    // �����ע�ͣ���ôֱ�ӷ���
    if (m_line_comment == rule || m_block_comment == rule) return true;

    // ѭ��ƥ�䣬ֱ����ע�ͺͶ���ע��һ��ƥ��ʧ��
    while (true)
    {
        bool match = false;
        int pin_offset = -1;
        if (m_line_comment != nullptr)
        {
            if (m_line_comment_skip.find(offset) == m_line_comment_skip.end())
            {
                if (AnalysisABnfRuleMatch(m_line_comment, parent, not_key
                    , line, col, offset
                    , pin_offset, true))
                    match = true;
                else
                    m_line_comment_skip.insert(offset);
            }
        }

        if (m_block_comment != nullptr)
        {
            if (m_block_comment_skip.find(offset) == m_block_comment_skip.end())
            {
                if (AnalysisABnfRuleMatch(m_block_comment, parent, not_key
                    , line, col, offset
                    , pin_offset, true))
                    match = true;
                else
                    m_block_comment_skip.insert(offset);
            }
        }

        if (!match) return true;
    }
}

// ����value_len����ƫ��
void ABnf::AnalysisOffset(int value_len, int& line, int& col, int& offset)
{
    while (true)
    {
        if (value_len == 0) break;
        if (offset >= m_file->GetLength()) break;

        char c = m_file->GetText()[offset];
        if (c == '\n')
        {
            ++line;
            col = 0;
        }
        else
        {
            ++col;
        }

        int byte_count = ABnfFile::GetByteCountOfOneWord(c);
        value_len -= byte_count;
        offset += byte_count;
    }
}

// ������һ��
bool ABnf::JumpToNextLine(int& line, int& col, int& offset)
{
    while (true)
    {
        if (offset >= m_file->GetLength()) break;

        char c = m_file->GetText()[offset];
        if (c == '\n')
        {
            ++line;
            col = 0;
            ++offset;

            return offset < m_file->GetLength();
        }
        else
        {
            ++col;
            offset += ABnfFile::GetByteCountOfOneWord(c);
        }
    }

    return false;
}

// ���и��ַ�������Ծ
void ABnf::AnalysisSkip(int& line, int& col, int& offset)
{
    while (offset < m_file->GetLength())
    {
        char c = m_file->GetText()[offset];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            return;

        if (c == '\n')
        {
            ++line;
            col = 0;
        }
        else
        {
            ++col;
        }
        ++offset;
    }
}
