
#ifndef _ALITTLE_ALITTLESCRIPTINDEX_H_
#define _ALITTLE_ALITTLESCRIPTINDEX_H_

#include <unordered_map>
#include <map>
#include <set>
#include <memory>
#include <vector>

class ABnfFile;
class ABnfElement;
class ABnfGuess;
using ABnfGuessPtr = std::shared_ptr<ABnfGuess>;
struct ABnfGuessError;
class ALittleScriptClassData;
class ALittleScriptStructData;
class ALittleScriptEnumData;
class ALittleScriptAccessData;
class ALittleScriptNamespaceNameDecElement;
class ALittleScriptClassDecElement;
class ALittleScriptStructDecElement;
class ALittleScriptEnumDecElement;
class ALittleScriptRootElement;
class ABnfProject;

struct RelayInfo
{
    std::set<RelayInfo*> be_used_set; // ����������
    std::set<RelayInfo*> use_set;    // ��������

    std::string path;
    std::set<std::string> relay_set;
    std::string rel_path;
};

class ALittleScriptIndex
{
protected:
    // ����ؼ���Ԫ�ض������ڿ����﷨������
    // Key1:�ļ�����Key2:Ԫ�ض���Value:����
    std::unordered_map<ABnfFile*, std::map<std::shared_ptr<ABnfElement>, std::vector<std::shared_ptr<ABnfGuess>>>> m_guess_type_map;
    std::unordered_map<ABnfFile*, std::map<std::shared_ptr<ABnfElement>, ABnfGuessError>> m_guess_error_map;
    // Key1:�ļ�����Key2:���ƣ�Value:����
    std::unordered_map<ABnfFile*, std::unordered_map<std::string, ALittleScriptClassData>> m_class_data_map;
    std::unordered_map<ABnfFile*, std::unordered_map<std::string, ALittleScriptStructData>> m_struct_data_map;
    std::unordered_map<ABnfFile*, std::unordered_map<std::string, ALittleScriptEnumData>> m_enum_data_map;

    // ��������������
    // string ��ʾ������
    std::unordered_map<std::string, std::map<std::shared_ptr<ALittleScriptNamespaceNameDecElement>, ALittleScriptAccessData>> m_all_data_map;
    // ���ݿ���Ȩ��������
    // string ��ʾ������
    std::unordered_map<std::string, ALittleScriptAccessData> m_global_access_map;
    std::unordered_map<std::string, ALittleScriptAccessData> m_namespace_access_map;
    std::unordered_map<ABnfFile*, ALittleScriptAccessData> m_file_access_map;

public:
    std::shared_ptr<ABnfGuess> sIntGuess;
    std::shared_ptr<ABnfGuess> sDoubleGuess;
    std::shared_ptr<ABnfGuess> sStringGuess;
    std::shared_ptr<ABnfGuess> sBoolGuess;
    std::shared_ptr<ABnfGuess> sLongGuess;
    std::shared_ptr<ABnfGuess> sAnyGuess;

    std::shared_ptr<ABnfGuess> sConstIntGuess;
    std::shared_ptr<ABnfGuess> sConstDoubleGuess;
    std::shared_ptr<ABnfGuess> sConstStringGuess;
    std::shared_ptr<ABnfGuess> sConstBoolGuess;
    std::shared_ptr<ABnfGuess> sConstLongGuess;
    std::shared_ptr<ABnfGuess> sConstAnyGuess;

    std::unordered_map<std::string, std::vector<std::shared_ptr<ABnfGuess>>> sPrimitiveGuessListMap;
    std::unordered_map<std::string, std::shared_ptr<ABnfGuess>> sPrimitiveGuessMap;

    std::vector<std::shared_ptr<ABnfGuess>> sConstNullGuess;

    // �������
    std::set<std::string> sCtrlKeyWord;

    ALittleScriptIndex();
    virtual ~ALittleScriptIndex() {}

    // ��ȡ�����б�
    const std::vector<std::shared_ptr<ABnfGuess>>* GetGuessTypeList(std::shared_ptr<ABnfElement> element);

    // �������
    void AddGuessTypeList(std::shared_ptr<ABnfElement> element, const std::vector<std::shared_ptr<ABnfGuess>>& guess_type_list);

    // ��ȡ���ʹ���
    ABnfGuessError GetGuessError(std::shared_ptr<ABnfElement> element);

    // ������ʹ���
    void AddGuessError(std::shared_ptr<ABnfElement> element, const ABnfGuessError& error);

public:
    // ɾ���ļ���
    static bool GetDeepFilePaths(ABnfProject* project, const std::string& cur_path, const std::string& parent_path, std::vector<std::string>& result, std::string& error);
    
    // ���Ҷ�������
    static ABnfGuessError FindDefineRelay(ABnfProject* project, const std::string& file_path, std::set<std::string>& result);

public:
    void FindNamespaceNameDecList(const std::string& namespace_name, std::unordered_map<std::string, std::shared_ptr<ALittleScriptNamespaceNameDecElement>>& result);

    void FindALittleNameDecList(ABnfElementType type
        , ABnfFile* file, const std::string& namespace_name, const std::string& name, bool find_in_global, std::vector<std::shared_ptr<ABnfElement>>& result);

    std::shared_ptr<ABnfElement> FindALittleNameDec(ABnfElementType type
        , ABnfFile* file, const std::string& namespace_name, const std::string& name, bool find_in_global)
    {
        std::vector<std::shared_ptr<ABnfElement>> result;
        FindALittleNameDecList(type, file, namespace_name, name, find_in_global, result);
        if (result.empty()) return nullptr;
        return result[0];
    }

    ABnfGuessError FindALittleStructGuessList(const std::string& namespace_name, const std::string& name, std::vector<ABnfGuessPtr>& guess_list);

    ABnfGuessError FindALittleStructGuess(const std::string& namespace_name, const std::string& name, ABnfGuessPtr& guess);

    ABnfGuessError FindALittleClassGuessList(const std::string& namespace_name, const std::string& name, std::vector<ABnfGuessPtr>& guess_list);

    void FindClassAttrList(std::shared_ptr<ALittleScriptClassDecElement> dec
        , int access_level, ClassAttrType attr_type, const std::string& name, std::vector<std::shared_ptr<ABnfElement>>& result);

    std::shared_ptr<ABnfElement> FindClassAttr(std::shared_ptr<ALittleScriptClassDecElement> dec
        , int access_level, ClassAttrType attr_type, const std::string& name);


    // �������������
    void AddClassData(std::shared_ptr<ALittleScriptClassDecElement> dec);

    // ��ȡ����������
    const ALittleScriptClassData* GetClassData(std::shared_ptr<ALittleScriptClassDecElement> dec);

    // ��ӽṹ������
    void AddStructData(std::shared_ptr<ALittleScriptStructDecElement> dec);

    // ��ȡ�ṹ������
    const ALittleScriptStructData* GetStructData(std::shared_ptr<ALittleScriptStructDecElement> dec);

    // ���ö������
    void AddEnumData(std::shared_ptr<ALittleScriptEnumDecElement> dec);

    // ��ȡö������
    const ALittleScriptEnumData* GetEnumData(std::shared_ptr<ALittleScriptEnumDecElement> dec);

    // ���������
    void AddRoot(std::shared_ptr<ALittleScriptRootElement> root);

    // �Ƴ�������
    void RemoveRoot(std::shared_ptr<ALittleScriptRootElement> root);
};

#endif // _ALITTLE_ALITTLESCRIPTINDEX_H_
