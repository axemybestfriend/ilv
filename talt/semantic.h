#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "scanner.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

// Перечисления
enum ObjectCategory {
    CAT_UNDEFINED,
    CAT_VARIABLE,
    CAT_FIELD,
    CAT_STRUCT_TYPE,
    CAT_TYPE
};

enum DataType {
    TYPE_UNDEFINED,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_STRUCT,
    TYPE_VOID
};

// Структура для поля структуры
struct FieldInfo {
    std::string name;
    DataType type;
    std::string structTypeName;

    FieldInfo(const std::string& n = "", DataType t = TYPE_UNDEFINED,
        const std::string& stn = "")
        : name(n), type(t), structTypeName(stn) {}
};

// Структура для типа структуры
struct StructTypeInfo {
    std::string name;
    std::vector<FieldInfo> fields;
    std::map<std::string, FieldInfo> fieldMap;

    bool addField(const std::string& fieldName, DataType type,
        const std::string& structTypeName = "") {
        if (fieldMap.find(fieldName) != fieldMap.end()) {
            return false;
        }

        FieldInfo field(fieldName, type, structTypeName);
        fields.push_back(field);
        fieldMap[fieldName] = field;
        return true;
    }

    FieldInfo* findField(const std::string& fieldName) {
        auto it = fieldMap.find(fieldName);
        if (it != fieldMap.end()) {
            return &it->second;
        }
        return nullptr;
    }
};

// Узел таблицы символов
class Symbol {
public:
    std::string name;
    ObjectCategory category;
    DataType type;
    std::string structTypeName;

    // Дополнительная информация
    bool isInitialized;
    bool isField;
    std::string parentStruct; // Для полей

    // Для функций (не используется в текущей версии)
    int paramCount;
    std::vector<DataType> paramTypes;

    Symbol* parentScope;
    std::vector<Symbol*> childScopes;

    Symbol(const std::string& n = "", ObjectCategory cat = CAT_UNDEFINED,
        DataType t = TYPE_UNDEFINED)
        : name(n), category(cat), type(t), structTypeName(""),
        isInitialized(false), isField(false), parentStruct(""),
        paramCount(0), parentScope(nullptr) {}

    bool isVariable() const { return category == CAT_VARIABLE; }
    bool isStructType() const { return category == CAT_STRUCT_TYPE; }
};

// Семантический анализатор
class SemanticAnalyzer {
private:
    Symbol* currentScope;
    Symbol* globalScope;

    std::map<std::string, StructTypeInfo> structTypes;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;

    // Вспомогательные методы


public:
    SemanticAnalyzer();
    ~SemanticAnalyzer();


    Symbol* createSymbol(const std::string& name, ObjectCategory cat,
        DataType type = TYPE_UNDEFINED);
    void addToCurrentScope(Symbol* symbol);


    bool isFloatType(DataType type) const;
    bool isNumericType(DataType type) const;
    bool isIntegerType(DataType type) const;
    DataType promoteType(DataType t1, DataType t2) const;

    // Управление областями видимости
    void enterScope();
    void leaveScope();
    Symbol* getCurrentScope() const { return currentScope; }

    // Объявление объектов
    bool declareVariable(const std::string& name, DataType type,
        const std::string& structTypeName = "",
        int line = 0, int col = 0);
    bool declareStructType(const std::string& name, int line = 0, int col = 0);
    bool addFieldToStruct(const std::string& structName,
        const std::string& fieldName, DataType type,
        const std::string& fieldStructType = "",
        int line = 0, int col = 0);

    // Поиск символов
    Symbol* findSymbol(const std::string& name) const;
    Symbol* findSymbolInCurrentScope(const std::string& name) const;
    StructTypeInfo* findStructType(const std::string& name) const;

    // Семантические проверки
    Symbol* checkIdentifier(const std::string& name,
        int line = 0, int col = 0);
    bool checkAssignment(Symbol* left, DataType rightType,
        int line = 0, int col = 0);
    DataType checkBinaryOperation(TokenType op, DataType leftType,
        DataType rightType,
        int line = 0, int col = 0);
    bool checkFieldAccess(Symbol* structVar, const std::string& fieldName,
        DataType* resultType = nullptr,
        int line = 0, int col = 0);
    bool checkForLoop(DataType initType, DataType condType, DataType incType,
        int line = 0, int col = 0);

    // Вспомогательные функции
    DataType tokenTypeToDataType(TokenType token) const;
    static std::string dataTypeToString(DataType type);
    static std::string categoryToString(ObjectCategory cat);

    // Работа с ошибками
    void addError(const std::string& error, int line = 0, int col = 0);
    void addWarning(const std::string& warning, int line = 0, int col = 0);
    void printErrors() const;
    void printWarnings() const;
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }

    // Вывод информации
    void printSymbolTable() const;
    void printStructTypes() const;

    // Очистка
    void clear();
};

#endif