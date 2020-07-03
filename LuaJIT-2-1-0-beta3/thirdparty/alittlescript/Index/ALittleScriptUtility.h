
#ifndef _ALITTLE_ALITTLESCRIPTUTILITY_H_
#define _ALITTLE_ALITTLESCRIPTUTILITY_H_

#include <string>
#include <memory>
#include <vector>

class ALittleScriptNumberElement;
class ALittleScriptGuessStruct;
class ALittleScriptModifierElement;
class ABnfElement;
struct ABnfGuessError;
class ABnfFile;
class ALittleScriptNamespaceDecElement;
class ALittleScriptNamespaceNameDecElement;
class ALittleScriptClassDecElement;
class ALittleScriptTemplateDecElement;

// �����������
enum class ClassAttrType
{
    VAR,            // ��Ա����
    FUN,            // ��Ա����
    GETTER,         // getter����
    SETTER,         // setter����
    STATIC,         // ��̬����
    TEMPLATE,       // ģ�����
};

// ����Ȩ������
enum class ClassAccessType
{
    PUBLIC,         // ȫ�ֿɷ���
    PROTECTED,      // ��������ɷ���
    PRIVATE,        // ����ɷ���
};

// Ԫ������
enum class ABnfElementType
{
    CLASS_NAME,         // ����
    ENUM_NAME,          // ö����
    STRUCT_NAME,        // �ṹ����
    INSTANCE_NAME,      // ������
    GLOBAL_METHOD,      // ȫ�ֺ���
    USING_NAME,         // using��
};

class ALittleScriptUtility
{
public:
	// �����ļ���
	static void CreateFolder(const std::string& path);

	// �ݹ鴴���ļ���
	static void CreateDeepFolder(const std::string& path);

    // �ж��ļ��Ƿ����
    static bool IsFileExist(const std::string& path);

    // �ж��ļ��Ƿ����
    static bool IsDirExist(const std::string& path);

    // ��ȡ�ļ����µ������ļ�
    enum class NameType
    {
        NT_ALL,
        NT_FILE,
        NT_DIR,
    };
    static void GetNameListInFolder(const std::string& path, NameType type, std::vector<std::string>& out);

    // �ж��ַ����ǲ�������ֵ
	static bool IsInt(const std::shared_ptr<ALittleScriptNumberElement>& element);

    // �����ϣֵ
	static int JSHash(const std::string& content);

	// ����ṹ��Ĺ�ϣֵ
	static int StructHash(const std::shared_ptr<ALittleScriptGuessStruct>& guess);

    // �����ж�
    static bool IsLanguageEnable(const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list);

    // �Ƿ�ʹ��ԭ��
    static bool IsNative(const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list);

    // ���await
    static bool CheckInvokeAwaitError(std::shared_ptr<ABnfElement> element, ABnfGuessError& error);

    // �ж��Ƿ����
    static bool CheckError(std::shared_ptr<ABnfElement> parent, const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list, ABnfGuessError& error);

    // ��ȡ�Ƿ���register
    static bool IsRegister(const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list);

    // ��ȡ�Ƿ���Const
    static bool IsConst(const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list);

    // ��ȡ�Ƿ���Nullable
    static bool IsNullable(const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list);

    // ��ȡЭ������
    static bool GetCoroutineType(const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list, std::string& out);

    // ��ȡЭ������
    static bool GetProtocolType(const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list, std::string& out);

    // ��ȡ��������
    static bool GetCommandDetail(const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list, std::string& cmd, std::string& desc);

    // ��ȡ����Ȩ������
    static ClassAccessType CalcAccessType(const std::vector<std::shared_ptr<ALittleScriptModifierElement>>& element_list);

    // ����Ȩ�޵ȼ�
    static const int sAccessOnlyPublic = 1;                    // ���Է���public�����Ժͷ���
    static const int sAccessProtectedAndPublic = 2;            // ���Է���public protected�����Ժͷ���
    static const int sAccessPrivateAndProtectedAndPublic = 3;  // ����public protected private�����Ժͷ���

    // ��ȡĳ��Ԫ�ص����������
    static std::shared_ptr<ALittleScriptNamespaceDecElement> GetNamespaceDec(ABnfFile* file);

    // ��ȡĳ��Ԫ�ص�������������
    static std::shared_ptr<ALittleScriptNamespaceNameDecElement> GetNamespaceNameDec(ABnfFile* file);

    // �ж�ĳ���ǲ���register
    static bool IsRegister(std::shared_ptr<ABnfElement> element);

    // ��ȡĳ��Ԫ�ص���������
    static const std::string& GetNamespaceName(std::shared_ptr<ABnfElement> element);

    // ��ȡ��������
    static const std::string& GetNamespaceName(ABnfFile* file);

    // ��ȡ���Ԫ�����ڵ���
    static std::shared_ptr<ALittleScriptClassDecElement> FindClassDecFromParent(std::shared_ptr<ABnfElement> dec);

    // ��ȡ���Ԫ�����ڵĺ���ģ��
    static std::shared_ptr<ALittleScriptTemplateDecElement> FindMethodTemplateDecFromParent(std::shared_ptr<ABnfElement> dec);

    // ����Ƿ��ھ�̬������
    static bool IsInClassStaticMethod(std::shared_ptr<ABnfElement> dec);
};

#endif // _ALITTLE_ALITTLESCRIPTUTILITY_H_
