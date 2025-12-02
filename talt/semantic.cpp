#include "semantic.h"
#include <iostream>
#include <iomanip>
#include <sstream>

SemanticAnalyzer::SemanticAnalyzer() {
    globalScope = new Symbol("global", CAT_TYPE, TYPE_VOID);
    currentScope = globalScope;

    // Добавляем встроенные типы
    Symbol* intType = new Symbol("int", CAT_TYPE, TYPE_INT);
    Symbol* shortType = new Symbol("short", CAT_TYPE, TYPE_SHORT);
    Symbol* longType = new Symbol("long", CAT_TYPE, TYPE_LONG);
    Symbol* floatType = new Symbol("float", CAT_TYPE, TYPE_FLOAT);
    Symbol* voidType = new Symbol("void", CAT_TYPE, TYPE_VOID);

    addToCurrentScope(intType);
    addToCurrentScope(shortType);
    addToCurrentScope(longType);
    addToCurrentScope(floatType);
    addToCurrentScope(voidType);
}

SemanticAnalyzer::~SemanticAnalyzer() {
    // Рекурсивное удаление символов
    std::function<void(Symbol*)> deleteSymbol = [&](Symbol* sym) {
        for (Symbol* child : sym->childScopes) {
            deleteSymbol(child);
        }
        delete sym;
    };
    deleteSymbol(globalScope);
}

Symbol* SemanticAnalyzer::createSymbol(const std::string& name,
    ObjectCategory cat, DataType type) {
    return new Symbol(name, cat, type);
}

void SemanticAnalyzer::addToCurrentScope(Symbol* symbol) {
    symbol->parentScope = currentScope;
    currentScope->childScopes.push_back(symbol);
}

void SemanticAnalyzer::enterScope() {
    Symbol* newScope = new Symbol("", CAT_TYPE, TYPE_VOID);
    newScope->parentScope = currentScope;
    currentScope->childScopes.push_back(newScope);
    currentScope = newScope;
}

void SemanticAnalyzer::leaveScope() {
    if (currentScope->parentScope) {
        currentScope = currentScope->parentScope;
    }
}

bool SemanticAnalyzer::declareVariable(const std::string& name, DataType type,
    const std::string& structTypeName,
    int line, int col) {
    // Проверка на повторное объявление
    if (findSymbolInCurrentScope(name)) {
        std::stringstream ss;
        ss << "Повторное объявление переменной '" << name
            << "' в строке " << line;
        addError(ss.str(), line, col);
        return false;
    }

    Symbol* var = createSymbol(name, CAT_VARIABLE, type);
    var->structTypeName = structTypeName;  // Сохраняем имя структуры
    var->isInitialized = false;
    addToCurrentScope(var);

    return true;
}

bool SemanticAnalyzer::declareStructType(const std::string& name,
    int line, int col) {
    // Проверка на повторное объявление
    if (structTypes.find(name) != structTypes.end()) {
        std::stringstream ss;
        ss << "Повторное объявление структуры '" << name
            << "' в строке " << line;
        addError(ss.str(), line, col);
        return false;
    }

    StructTypeInfo structInfo;
    structInfo.name = name;
    structTypes[name] = structInfo;

    // Также добавляем как символ
    Symbol* structSym = createSymbol(name, CAT_STRUCT_TYPE, TYPE_STRUCT);
    addToCurrentScope(structSym);

    return true;
}

bool SemanticAnalyzer::addFieldToStruct(const std::string& structName,
    const std::string& fieldName,
    DataType type,
    const std::string& fieldStructType,
    int line, int col) {
    auto it = structTypes.find(structName);
    if (it == structTypes.end()) {
        std::stringstream ss;
        ss << "Структура '" << structName << "' не найдена при добавлении поля '"
            << fieldName << "' в строке " << line;
        addError(ss.str(), line, col);
        return false;
    }

    if (!it->second.addField(fieldName, type, fieldStructType)) {
        std::stringstream ss;
        ss << "Повторное объявление поля '" << fieldName
            << "' в структуре '" << structName << "' в строке " << line;
        addError(ss.str(), line, col);
        return false;
    }

    return true;
}

Symbol* SemanticAnalyzer::findSymbol(const std::string& name) const {
    Symbol* scope = currentScope;

    while (scope) {
        for (Symbol* sym : scope->childScopes) {
            if (sym->name == name && sym->category != CAT_TYPE) {
                return sym;
            }
        }
        scope = scope->parentScope;
    }

    return nullptr;
}

Symbol* SemanticAnalyzer::findSymbolInCurrentScope(const std::string& name) const {
    for (Symbol* sym : currentScope->childScopes) {
        if (sym->name == name) {
            return sym;
        }
    }
    return nullptr;
}

StructTypeInfo* SemanticAnalyzer::findStructType(const std::string& name) const {
    auto it = structTypes.find(name);
    if (it != structTypes.end()) {
        return const_cast<StructTypeInfo*>(&it->second);
    }
    return nullptr;
}

Symbol* SemanticAnalyzer::checkIdentifier(const std::string& name,
    int line, int col) {
    Symbol* symbol = findSymbol(name);

    if (!symbol) {
        // Проверяем, не является ли это именем структуры
        if (findStructType(name)) {
            std::stringstream ss;
            ss << "'" << name << "' является именем структуры, а не переменной в строке " << line;
            addError(ss.str(), line, col);
            return nullptr;
        }

        std::stringstream ss;
        ss << "Идентификатор '" << name << "' не объявлен в строке " << line;
        addError(ss.str(), line, col);
        return nullptr;
    }

    if (symbol->category == CAT_TYPE && symbol->type != TYPE_STRUCT) {
        std::stringstream ss;
        ss << "'" << name << "' является типом, а не переменной в строке " << line;
        addError(ss.str(), line, col);
        return nullptr;
    }

    return symbol;
}

bool SemanticAnalyzer::isIntegerType(DataType type) const {
    return type == TYPE_SHORT || type == TYPE_INT || type == TYPE_LONG;
}

bool SemanticAnalyzer::isFloatType(DataType type) const {
    return type == TYPE_FLOAT;
}

bool SemanticAnalyzer::isNumericType(DataType type) const {
    return isIntegerType(type) || isFloatType(type);
}

DataType SemanticAnalyzer::promoteType(DataType t1, DataType t2) const {
    if (t1 == TYPE_FLOAT || t2 == TYPE_FLOAT) {
        return TYPE_FLOAT;
    }

    if (t1 == TYPE_LONG || t2 == TYPE_LONG) {
        return TYPE_LONG;
    }
    if (t1 == TYPE_INT || t2 == TYPE_INT) {
        return TYPE_INT;
    }

    return TYPE_SHORT;
}

bool SemanticAnalyzer::checkAssignment(Symbol* left, DataType rightType,
    int line, int col) {
    if (!left) {
        addError("Левая часть присваивания не определена", line, col);
        return false;
    }

    if (left->category != CAT_VARIABLE) {
        std::stringstream ss;
        ss << "Левая часть присваивания должна быть переменной в строке " << line;
        addError(ss.str(), line, col);
        return false;
    }

    DataType leftType = left->type;

    // Специальная обработка для структурных типов
    if (leftType == TYPE_STRUCT) {
        if (rightType == TYPE_STRUCT) {
            // Для структур требуем, чтобы имена типов совпадали
            if (!left->structTypeName.empty()) {
                Symbol* rightSymbol = findSymbolInCurrentScope("..."); // Нужен доступ к правой части
                // Упрощённая проверка: если обе стороны - структуры
                return true;
            }
        }
        return false;
    }

    if (leftType == rightType) {
        return true;
    }

    if (leftType == TYPE_FLOAT && isIntegerType(rightType)) {
        std::stringstream ss;
        ss << "Неявное приведение целого типа к float в строке " << line;
        addWarning(ss.str(), line, col);
        return true;
    }

    if (isIntegerType(leftType) && rightType == TYPE_FLOAT) {
        std::stringstream ss;
        ss << "Неявное приведение float к целому типу в строке " << line
            << " (потеря точности)";
        addError(ss.str(), line, col);
        return false;
    }

    if (isIntegerType(leftType) && isIntegerType(rightType)) {
        int leftSize = 0, rightSize = 0;
        if (leftType == TYPE_SHORT) leftSize = 2;
        else if (leftType == TYPE_INT) leftSize = 4;
        else if (leftType == TYPE_LONG) leftSize = 4;

        if (rightType == TYPE_SHORT) rightSize = 2;
        else if (rightType == TYPE_INT) rightSize = 4;
        else if (rightType == TYPE_LONG) rightSize = 4;

        if (rightSize > leftSize) {
            std::stringstream ss;
            ss << "Возможна потеря данных при присваивании в строке " << line;
            addWarning(ss.str(), line, col);
        }
        return true;
    }

    std::stringstream ss;
    ss << "Несовместимые типы в присваивании: "
        << dataTypeToString(leftType) << " и " << dataTypeToString(rightType)
        << " в строке " << line;
    addError(ss.str(), line, col);
    return false;
}

DataType SemanticAnalyzer::checkBinaryOperation(TokenType op,
    DataType leftType,
    DataType rightType,
    int line, int col) {
    if (op == TK_PLUS || op == TK_MINUS || op == TK_MUL || op == TK_DIV) {
        if (!isNumericType(leftType) || !isNumericType(rightType)) {
            std::stringstream ss;
            ss << "Арифметическая операция применима только к числовым типам в строке " << line;
            addError(ss.str(), line, col);
            return TYPE_UNDEFINED;
        }
        return promoteType(leftType, rightType);
    }

    if (op == TK_EQ || op == TK_NE || op == TK_LT || op == TK_LE ||
        op == TK_GT || op == TK_GE) {
        if (!isNumericType(leftType) || !isNumericType(rightType)) {
            std::stringstream ss;
            ss << "Операция сравнения применима только к числовым типам в строке " << line;
            addError(ss.str(), line, col);
            return TYPE_UNDEFINED;
        }
        return TYPE_INT;
    }

    if (op == TK_BIT_AND || op == TK_BIT_OR || op == TK_BIT_XOR ||
        op == TK_SHL || op == TK_SHR) {
        if (!isIntegerType(leftType) || !isIntegerType(rightType)) {
            std::stringstream ss;
            ss << "Побитовая операция применима только к целочисленным типам в строке " << line;
            addError(ss.str(), line, col);
            return TYPE_UNDEFINED;
        }
        return promoteType(leftType, rightType);
    }

    if (op == TK_MOD) {
        if (!isIntegerType(leftType) || !isIntegerType(rightType)) {
            std::stringstream ss;
            ss << "Операция % применима только к целочисленным типам в строке " << line;
            addError(ss.str(), line, col);
            return TYPE_UNDEFINED;
        }
        return promoteType(leftType, rightType);
    }

    return TYPE_UNDEFINED;
}

bool SemanticAnalyzer::checkFieldAccess(Symbol* structVar,
    const std::string& fieldName,
    DataType* resultType,
    int line, int col) {
    if (!structVar) {
        addError("Переменная не определена", line, col);
        return false;
    }

    if (structVar->type != TYPE_STRUCT) {
        std::stringstream ss;
        ss << "Доступ к полю возможен только для переменных структурного типа в строке " << line;
        addError(ss.str(), line, col);
        return false;
    }

    if (structVar->structTypeName.empty()) {
        std::stringstream ss;
        ss << "Тип структуры не указан для переменной '" << structVar->name << "' в строке " << line;
        addError(ss.str(), line, col);
        return false;
    }

    StructTypeInfo* structInfo = findStructType(structVar->structTypeName);
    if (!structInfo) {
        std::stringstream ss;
        ss << "Тип структуры '" << structVar->structTypeName
            << "' не найден в строке " << line;
        addError(ss.str(), line, col);
        return false;
    }

    FieldInfo* field = structInfo->findField(fieldName);
    if (!field) {
        std::stringstream ss;
        ss << "Поле '" << fieldName << "' не найдено в структуре '"
            << structInfo->name << "' в строке " << line;
        addError(ss.str(), line, col);
        return false;
    }

    if (resultType) {
        *resultType = field->type;
    }

    return true;
}

bool SemanticAnalyzer::checkForLoop(DataType initType, DataType condType,
    DataType incType, int line, int col) {
    bool hasError = false;

    if (initType != TYPE_VOID && !isNumericType(initType)) {
        std::stringstream ss;
        ss << "Тип инициализации в цикле for должен быть числовым в строке " << line;
        addError(ss.str(), line, col);
        hasError = true;
    }

    if (condType != TYPE_UNDEFINED && !isNumericType(condType)) {
        std::stringstream ss;
        ss << "Тип условия в цикле for должен быть числовым в строке " << line;
        addError(ss.str(), line, col);
        hasError = true;
    }

    if (incType != TYPE_UNDEFINED && !isNumericType(incType)) {
        std::stringstream ss;
        ss << "Тип инкремента в цикле for должен быть числовым в строке " << line;
        addError(ss.str(), line, col);
        hasError = true;
    }

    return !hasError;
}

DataType SemanticAnalyzer::tokenTypeToDataType(TokenType token) const {
    switch (token) {
    case TK_INT: return TYPE_INT;
    case TK_SHORT: return TYPE_SHORT;
    case TK_LONG: return TYPE_LONG;
    case TK_FLOAT: return TYPE_FLOAT;
    case TK_VOID: return TYPE_VOID;
    default: return TYPE_UNDEFINED;
    }
}

std::string SemanticAnalyzer::dataTypeToString(DataType type) {
    switch (type) {
    case TYPE_UNDEFINED: return "undefined";
    case TYPE_SHORT: return "short";
    case TYPE_INT: return "int";
    case TYPE_LONG: return "long";
    case TYPE_FLOAT: return "float";
    case TYPE_STRUCT: return "struct";
    case TYPE_VOID: return "void";
    default: return "unknown";
    }
}

std::string SemanticAnalyzer::categoryToString(ObjectCategory cat) {
    switch (cat) {
    case CAT_UNDEFINED: return "undefined";
    case CAT_VARIABLE: return "variable";
    case CAT_FIELD: return "field";
    case CAT_STRUCT_TYPE: return "struct_type";
    case CAT_TYPE: return "type";
    default: return "unknown";
    }
}

void SemanticAnalyzer::addError(const std::string& error, int line, int col) {
    std::stringstream ss;
    ss << "[ОШИБКА] ";
    if (line > 0) ss << "Строка " << line;
    if (col > 0) ss << ":" << col;
    if (line > 0 || col > 0) ss << ": ";
    ss << error;
    errors.push_back(ss.str());
}

void SemanticAnalyzer::addWarning(const std::string& warning, int line, int col) {
    std::stringstream ss;
    ss << "[ПРЕДУПРЕЖДЕНИЕ] ";
    if (line > 0) ss << "Строка " << line;
    if (col > 0) ss << ":" << col;
    if (line > 0 || col > 0) ss << ": ";
    ss << warning;
    warnings.push_back(ss.str());
}

void SemanticAnalyzer::printErrors() const {
    if (errors.empty()) {
        std::cout << "Ошибок не обнаружено.\n";
        return;
    }

    std::cout << "\n=== ОШИБКИ СЕМАНТИЧЕСКОГО АНАЛИЗА ===\n";
    for (const auto& error : errors) {
        std::cout << error << std::endl;
    }
}

void SemanticAnalyzer::printWarnings() const {
    if (warnings.empty()) {
        return;
    }

    std::cout << "\n=== ПРЕДУПРЕЖДЕНИЯ ===\n";
    for (const auto& warning : warnings) {
        std::cout << warning << std::endl;
    }
}

void SemanticAnalyzer::printSymbolTable() const {
    std::function<void(const Symbol*, int)> printScope =
        [&](const Symbol* scope, int depth) {
        std::string indent(depth * 2, ' ');

        if (scope->name.empty()) {
            std::cout << indent << "Scope (уровень " << depth << "):\n";
        }
        else {
            std::cout << indent << "Scope '" << scope->name << "':\n";
        }

        for (const Symbol* sym : scope->childScopes) {
            std::cout << indent << "  " << sym->name
                << " [" << categoryToString(sym->category)
                << ", " << dataTypeToString(sym->type);

            if (!sym->structTypeName.empty()) {
                std::cout << ", struct: " << sym->structTypeName;
            }

            if (sym->category == CAT_VARIABLE && sym->isInitialized) {
                std::cout << ", initialized";
            }

            std::cout << "]\n";

            if (!sym->childScopes.empty()) {
                printScope(sym, depth + 2);
            }
        }
    };

    std::cout << "\n=== ТАБЛИЦА СИМВОЛОВ ===\n";
    printScope(globalScope, 0);
}

void SemanticAnalyzer::printStructTypes() const {
    if (structTypes.empty()) {
        std::cout << "Определения структур отсутствуют.\n";
        return;
    }

    std::cout << "\n=== ОПРЕДЕЛЕНИЯ СТРУКТУР ===\n";
    for (const auto& pair : structTypes) {
        std::cout << "struct " << pair.first << " {\n";
        for (const auto& field : pair.second.fields) {
            std::cout << "    " << dataTypeToString(field.type)
                << " " << field.name << ";\n";
        }
        std::cout << "}\n";
    }
}

void SemanticAnalyzer::clear() {
    errors.clear();
    warnings.clear();
    structTypes.clear();

    std::function<void(Symbol*)> clearScope = [&](Symbol* scope) {
        for (Symbol* child : scope->childScopes) {
            clearScope(child);
        }
        delete scope;
    };

    clearScope(globalScope);

    globalScope = new Symbol("global", CAT_TYPE, TYPE_VOID);
    currentScope = globalScope;

    Symbol* intType = new Symbol("int", CAT_TYPE, TYPE_INT);
    Symbol* shortType = new Symbol("short", CAT_TYPE, TYPE_SHORT);
    Symbol* longType = new Symbol("long", CAT_TYPE, TYPE_LONG);
    Symbol* floatType = new Symbol("float", CAT_TYPE, TYPE_FLOAT);
    Symbol* voidType = new Symbol("void", CAT_TYPE, TYPE_VOID);

    addToCurrentScope(intType);
    addToCurrentScope(shortType);
    addToCurrentScope(longType);
    addToCurrentScope(floatType);
    addToCurrentScope(voidType);
}
