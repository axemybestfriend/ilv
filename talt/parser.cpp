#include "parser.h"
#include <iostream>
#include <sstream>
#include <cctype>

// ==================== Parser Implementation ====================

Parser::Parser(Scanner& sc, SemanticAnalyzer& sem)
    : scanner(sc), semantic(sem) {
    currentToken = scanner.getNextToken();
    hasError = false;
}

void Parser::advance() {
    currentToken = scanner.getNextToken();
}

bool Parser::match(TokenType expected) {
    if (currentToken.type == expected) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType expected) const {
    return currentToken.type == expected;
}

Token Parser::peekToken() {
    return scanner.peekNextToken();
}

void Parser::error(const std::string& message) {
    std::stringstream ss;
    ss << "Синтаксическая ошибка в строке " << currentToken.line
        << ":" << currentToken.column << ": " << message;
    std::cerr << ss.str() << std::endl;
    hasError = true;
}

DataType Parser::parseType(std::string* structTypeName) {
    DataType type = TYPE_UNDEFINED;

    if (check(TK_INT)) {
        type = TYPE_INT;
        match(TK_INT);
    }
    else if (check(TK_SHORT)) {
        type = TYPE_SHORT;
        match(TK_SHORT);
    }
    else if (check(TK_LONG)) {
        type = TYPE_LONG;
        match(TK_LONG);
    }
    else if (check(TK_FLOAT)) {
        type = TYPE_FLOAT;
        match(TK_FLOAT);
    }
    else if (check(TK_STRUCT)) {
        type = TYPE_STRUCT;
        match(TK_STRUCT);
        if (check(TK_IDENT)) {
            if (structTypeName) {
                *structTypeName = currentToken.lexeme;
            }
            advance();
        }
    }
    else if (check(TK_VOID)) {
        type = TYPE_VOID;
        match(TK_VOID);
    }
    else if (check(TK_IDENT)) {
        type = TYPE_STRUCT;
        if (structTypeName) {
            *structTypeName = currentToken.lexeme;
        }
        advance();
    }

    return type;
}
std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = parseProgram();

    if (!match(TK_EOF)) {
        error("Ожидался конец файла");
    }

    return program;
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    program->line = currentToken.line;
    program->column = currentToken.column;

    semantic.enterScope();

    while (!check(TK_EOF)) {
        auto decl = parseDeclaration();
        if (decl) {
            program->declarations.push_back(std::move(decl));
        }
        else {
            // Пропускаем токены до следующего объявления
            while (!check(TK_EOF) &&
                !check(TK_INT) && !check(TK_SHORT) &&
                !check(TK_LONG) && !check(TK_FLOAT) &&
                !check(TK_STRUCT) && !check(TK_VOID)) {
                advance();
            }
        }
    }

    semantic.leaveScope();
    return program;
}

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    if (check(TK_STRUCT)) {
        return parseStructDeclaration();
    }

    // Сохраняем начальную позицию
    Token startToken = currentToken;

    // Пробуем распарсить тип
    std::string structTypeName;
    DataType type = parseType(&structTypeName);

    if (type == TYPE_UNDEFINED) {
        // Возвращаемся к началу
        currentToken = startToken;
        // Это может быть выражение
        return parseStatement();
    }

    if (!check(TK_IDENT)) {
        error("Ожидался идентификатор после типа");
        return nullptr;
    }

    std::string identName = currentToken.lexeme;

    // Смотрим следующий токен
    Token nextToken = peekToken();

    if (nextToken.type == TK_LPAREN) {
        // Это функция - нужно вернуться к началу
        currentToken = startToken;
        return parseFunctionDeclaration(type);
    }
    else {
        // Это переменная
        return parseVariableDeclaration(type, structTypeName);
    }
}


std::unique_ptr<StructDeclNode> Parser::parseStructDeclaration() {
    auto structDecl = std::make_unique<StructDeclNode>();
    structDecl->line = currentToken.line;
    structDecl->column = currentToken.column;

    match(TK_STRUCT);

    if (!check(TK_IDENT)) {
        error("Ожидалось имя структуры");
        return nullptr;
    }

    structDecl->name = currentToken.lexeme;
    advance();

    if (!match(TK_LBRACE)) {
        error("Ожидалась '{' после имени структуры");
        return nullptr;
    }

    while (!check(TK_RBRACE) && !check(TK_EOF)) {
        DataType fieldType = parseType();
        if (fieldType == TYPE_UNDEFINED) {
            error("Ожидался тип поля структуры");
            return nullptr;
        }

        if (!check(TK_IDENT)) {
            error("Ожидалось имя поля структуры");
            return nullptr;
        }

        std::string fieldName = currentToken.lexeme;
        advance();

        if (!match(TK_SEMICOLON)) {
            error("Ожидалась ';' после объявления поля");
            return nullptr;
        }

        structDecl->fields.push_back({ fieldName, fieldType });
    }

    if (!match(TK_RBRACE)) {
        error("Ожидалась '}' после объявления полей структуры");
        return nullptr;
    }

    if (!match(TK_SEMICOLON)) {
        error("Ожидалась ';' после определения структуры");
        return nullptr;
    }

    return structDecl;
}

std::unique_ptr<FunctionNode> Parser::parseFunctionDeclaration(DataType returnType) {
    auto funcDecl = std::make_unique<FunctionNode>();
    funcDecl->line = currentToken.line;
    funcDecl->column = currentToken.column;
    funcDecl->returnType = returnType;

    if (!check(TK_IDENT)) {
        error("Ожидалось имя функции");
        return nullptr;
    }

    funcDecl->name = currentToken.lexeme;
    advance();

    if (!match(TK_LPAREN)) {
        error("Ожидалась '(' после имени функции");
        return nullptr;
    }

    if (!match(TK_RPAREN)) {
        error("Ожидалась ')' после параметров функции");
        return nullptr;
    }

    // Тело функции
    semantic.enterScope(); // Новая область видимости для функции

    if (!check(TK_LBRACE)) {
        error("Ожидалось '{' для тела функции");
        semantic.leaveScope();
        return nullptr;
    }

    auto body = parseBlock();
    if (!body) {
        error("Ожидалось тело функции");
        semantic.leaveScope();
        return nullptr;
    }

    funcDecl->body = std::move(body);
    semantic.leaveScope();

    return funcDecl;
}

std::unique_ptr<VarDeclNode> Parser::parseVariableDeclaration(DataType type, const std::string& structTypeName) {
    auto varDecl = std::make_unique<VarDeclNode>();
    varDecl->line = currentToken.line;
    varDecl->column = currentToken.column;
    varDecl->type = type;
    varDecl->structName = structTypeName;

    if (!check(TK_IDENT)) {
        error("Ожидалось имя переменной");
        return nullptr;
    }

    varDecl->name = currentToken.lexeme;
    advance();

    if (match(TK_ASSIGN)) {
        varDecl->initValue = parseExpression();
        if (!varDecl->initValue) {
            error("Ожидалось выражение инициализации");
            return nullptr;
        }
    }

    if (!match(TK_SEMICOLON)) {
        error("Ожидалась ';' после объявления переменной");
        return nullptr;
    }

    return varDecl;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    // ПЕРВОЕ: проверяем, не объявление ли это переменной
    // Сохраняем текущую позицию
    std::streampos savedPos = scanner.getPosition();
    Token savedToken = currentToken;

    // Пробуем распарсить тип
    DataType type = parseType();
    if (type != TYPE_UNDEFINED) {
        // Успешно распарсили тип - проверяем, что дальше
        if (check(TK_IDENT)) {
            std::string identName = currentToken.lexeme;
            Token nextToken = peekToken();

            if (nextToken.type == TK_LPAREN) {
                // Это объявление функции - не может быть внутри другого блока
                // Возвращаемся назад и парсим как выражение
                scanner.setPosition(savedPos);
                currentToken = savedToken;
                // Будем парсить как обычное выражение ниже
            }
            else {
                // Это объявление переменной!
                return parseVariableDeclaration(type, "");
            }
        }
    }
    else {
        // Не тип - восстанавливаем позицию
        scanner.setPosition(savedPos);
        currentToken = savedToken;
    }

    // ВТОРОЕ: если не объявление переменной, то это оператор
    if (check(TK_FOR)) {
        return parseForLoop();
    }

    if (check(TK_RETURN)) {
        return parseReturnStatement();
    }

    if (check(TK_LBRACE)) {
        return parseBlock();
    }

    // ТРЕТЬЕ: это может быть присваивание или выражение
    auto expr = parseExpression();
    if (!expr) {
        return nullptr;
    }

    if (!match(TK_SEMICOLON)) {
        error("Ожидалась ';' после выражения");
        return nullptr;
    }

    return expr;
}

std::unique_ptr<ForLoopNode> Parser::parseForLoop() {
    auto forLoop = std::make_unique<ForLoopNode>();
    forLoop->line = currentToken.line;
    forLoop->column = currentToken.column;

    match(TK_FOR);

    if (!match(TK_LPAREN)) {
        error("Ожидалась '(' после for");
        return nullptr;
    }

    // Инициализация (может быть объявлением переменной или выражением)
    // Сохраняем позицию для отката
    std::streampos savedPos = scanner.getPosition();
    Token savedToken = currentToken;

    DataType type = parseType();
    if (type != TYPE_UNDEFINED && check(TK_IDENT)) {
        // Это объявление переменной в for
        auto varDecl = parseVariableDeclaration(type, "");
        if (varDecl) {
            forLoop->init = std::move(varDecl);
        }
        else {
            return nullptr;
        }
    }
    else {
        // Это выражение или пустая инициализация
        scanner.setPosition(savedPos);
        currentToken = savedToken;

        if (check(TK_SEMICOLON)) {
            match(TK_SEMICOLON);
        }
        else {
            forLoop->init = parseExpression();
            if (!match(TK_SEMICOLON)) {
                error("Ожидалась ';' после инициализации for");
                return nullptr;
            }
        }
    }

    // Остальная часть функции остается без изменений...
    // Условие (может быть пустым)
    if (check(TK_SEMICOLON)) {
        match(TK_SEMICOLON);
    }
    else {
        forLoop->condition = parseExpression();
        if (!match(TK_SEMICOLON)) {
            error("Ожидалась ';' после условия for");
            return nullptr;
        }
    }

    // Инкремент (может быть пустым)
    if (check(TK_RPAREN)) {
        match(TK_RPAREN);
    }
    else {
        forLoop->increment = parseExpression();
        if (!match(TK_RPAREN)) {
            error("Ожидалась ')' после инкремента for");
            return nullptr;
        }
    }

    // Тело цикла
    forLoop->body = parseStatement();
    if (!forLoop->body) {
        error("Ожидалось тело цикла for");
        return nullptr;
    }

    return forLoop;
}

std::unique_ptr<ReturnNode> Parser::parseReturnStatement() {
    auto returnNode = std::make_unique<ReturnNode>();
    returnNode->line = currentToken.line;
    returnNode->column = currentToken.column;

    match(TK_RETURN);

    if (!check(TK_SEMICOLON)) {
        returnNode->expression = parseExpression();
        if (!returnNode->expression) {
            error("Ожидалось выражение после return");
            return nullptr;
        }
    }

    if (!match(TK_SEMICOLON)) {
        error("Ожидалась ';' после return");
        return nullptr;
    }

    return returnNode;
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    auto block = std::make_unique<BlockNode>();
    block->line = currentToken.line;
    block->column = currentToken.column;

    match(TK_LBRACE);

    semantic.enterScope(); // Новая область видимости

    while (!check(TK_RBRACE) && !check(TK_EOF)) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
        else {
            // Пропускаем токены до следующего оператора
            while (!check(TK_RBRACE) && !check(TK_EOF) &&
                !check(TK_SEMICOLON) && !check(TK_LBRACE) &&
                !check(TK_FOR) && !check(TK_RETURN)) {
                advance();
            }

            if (check(TK_SEMICOLON)) {
                advance(); // Пропускаем ошибочную ;
            }
        }
    }

    if (!match(TK_RBRACE)) {
        error("Ожидалась '}'");
        semantic.leaveScope();
        return nullptr;
    }

    semantic.leaveScope(); // Выход из области видимости

    return block;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parseLogicalOr();

    if (check(TK_ASSIGN)) {
        // Это присваивание
        auto assign = std::make_unique<AssignNode>();
        assign->line = currentToken.line;
        assign->column = currentToken.column;

        // Проверяем, является ли левая часть переменной или доступом к полю
        if (auto varNode = dynamic_cast<VarNode*>(left.get())) {
            assign->varName = varNode->name;
            assign->fieldName = varNode->fieldName;
        }
        else {
            error("Левая часть присваивания должна быть переменной");
            return nullptr;
        }

        match(TK_ASSIGN);
        assign->expression = parseExpression();
        if (!assign->expression) {
            error("Ожидалось выражение в правой части присваивания");
            return nullptr;
        }

        return assign;
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalOr() {
    auto node = parseLogicalAnd();

    while (check(TK_BIT_OR)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseLogicalAnd();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto node = parseBitwiseOr();

    while (check(TK_BIT_AND)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseBitwiseOr();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseBitwiseOr() {
    auto node = parseBitwiseXor();

    while (check(TK_BIT_OR)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseBitwiseXor();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseBitwiseXor() {
    auto node = parseBitwiseAnd();

    while (check(TK_BIT_XOR)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseBitwiseAnd();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseBitwiseAnd() {
    auto node = parseEquality();

    while (check(TK_BIT_AND)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseEquality();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseEquality() {
    auto node = parseRelational();

    while (check(TK_EQ) || check(TK_NE)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseRelational();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseRelational() {
    auto node = parseShift();

    while (check(TK_LT) || check(TK_LE) || check(TK_GT) || check(TK_GE)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseShift();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseShift() {
    auto node = parseAdditive();

    while (check(TK_SHL) || check(TK_SHR)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseAdditive();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseAdditive() {
    auto node = parseMultiplicative();

    while (check(TK_PLUS) || check(TK_MINUS)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseMultiplicative();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicative() {
    auto node = parseUnary();

    while (check(TK_MUL) || check(TK_DIV) || check(TK_MOD)) {
        auto opNode = std::make_unique<BinaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        opNode->left = std::move(node);
        advance();
        opNode->right = parseUnary();
        node = std::move(opNode);
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseUnary() {
    if (check(TK_PLUS) || check(TK_MINUS) || check(TK_BIT_NOT)) {
        auto opNode = std::make_unique<UnaryOpNode>();
        opNode->line = currentToken.line;
        opNode->column = currentToken.column;
        opNode->op = currentToken.type;
        advance();
        opNode->operand = parseUnary();
        return opNode;
    }

    return parsePostfix();
}

std::unique_ptr<ASTNode> Parser::parsePostfix() {
    auto node = parsePrimary();

    while (check(TK_DOT)) {
        // std::cout << "DEBUG: Found DOT token" << std::endl;

        match(TK_DOT); // Пропускаем '.'

        if (!check(TK_IDENT)) {
            error("Ожидалось имя поля после '.'");
            return nullptr;
        }

        // Проверяем, является ли node переменной
        if (auto varNode = dynamic_cast<VarNode*>(node.get())) {
            // std::cout << "DEBUG: It's a VarNode: " << varNode->name << std::endl;

            // Создаем новый VarNode для доступа к полю
            auto fieldAccess = std::make_unique<VarNode>();
            fieldAccess->line = currentToken.line;
            fieldAccess->column = currentToken.column;
            fieldAccess->name = varNode->name;
            fieldAccess->fieldName = currentToken.lexeme;
            // std::cout << "DEBUG: Creating field access: " << varNode->name << "." << currentToken.lexeme << std::endl;
            advance();
            node = std::move(fieldAccess);
        }
        else {
            // Пытаемся обработать как выражение с полем
            // Например: (expression).field
            error("Доступ к полям возможен только для переменных");
            return nullptr;
        }
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (check(TK_IDENT)) {
        auto varNode = std::make_unique<VarNode>();
        varNode->line = currentToken.line;
        varNode->column = currentToken.column;
        varNode->name = currentToken.lexeme;
        advance();
        return varNode;
    }

    if (check(TK_INT_CONST)) {
        auto constNode = std::make_unique<ConstNode>();
        constNode->line = currentToken.line;
        constNode->column = currentToken.column;
        constNode->type = TYPE_INT;
        constNode->value = currentToken.lexeme;
        advance();
        return constNode;
    }

    if (check(TK_FLOAT_CONST)) {
        auto constNode = std::make_unique<ConstNode>();
        constNode->line = currentToken.line;
        constNode->column = currentToken.column;
        constNode->type = TYPE_FLOAT;
        constNode->value = currentToken.lexeme;
        advance();
        return constNode;
    }

    if (match(TK_LPAREN)) {
        auto expr = parseExpression();
        if (!match(TK_RPAREN)) {
            error("Ожидалась ')'");
            return nullptr;
        }
        return expr;
    }

    error("Ожидался идентификатор, константа или выражение в скобках");
    return nullptr;
}

// ==================== AST Node Implementations ====================

void ProgramNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "Program:\n";
    for (const auto& decl : declarations) {
        if (decl) decl->print(indent + 2);
    }
}

DataType ProgramNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    for (const auto& decl : declarations) {
        decl->checkSemantics(sem, currentSymbol);
    }
    return TYPE_VOID;
}

void StructDeclNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "Struct " << name << ":\n";
    for (const auto& field : fields) {
        std::cout << spaces << "  " << field.first << ": "
            << SemanticAnalyzer::dataTypeToString(field.second) << "\n";
    }
}

DataType StructDeclNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    if (!sem.declareStructType(name, line, column)) {
        return TYPE_UNDEFINED;
    }

    for (const auto& field : fields) {
        if (!sem.addFieldToStruct(name, field.first, field.second, "", line, column)) {
            return TYPE_UNDEFINED;
        }
    }

    return TYPE_STRUCT;
}

void FunctionNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "Function " << name << ": "
        << SemanticAnalyzer::dataTypeToString(returnType) << "\n";
    if (body) {
        body->print(indent + 2);
    }
}

DataType FunctionNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    // В текущей версии просто проверяем тело функции
    if (body) {
        return body->checkSemantics(sem, currentSymbol);
    }
    return returnType;
}

void VarDeclNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "VarDecl " << name << ": ";

    if (type == TYPE_STRUCT && !structName.empty()) {
        std::cout << "struct " << structName;
    }
    else {
        std::cout << SemanticAnalyzer::dataTypeToString(type);
    }

    if (initValue) {
        std::cout << " =\n";
        initValue->print(indent + 2);
    }
    else {
        std::cout << "\n";
    }
}

DataType VarDeclNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    if (type == TYPE_STRUCT && !structName.empty()) {
        if (!sem.findStructType(structName)) {
            sem.addError("Тип структуры '" + structName + "' не определен", line, column);
            return TYPE_UNDEFINED;
        }
    }

    if (!sem.declareVariable(name, type, structName, line, column)) {
        return TYPE_UNDEFINED;
    }

    if (initValue) {
        DataType initType = initValue->checkSemantics(sem, currentSymbol);
        Symbol* varSymbol = sem.findSymbol(name);

        if (varSymbol && initType != TYPE_UNDEFINED) {
            if (!sem.checkAssignment(varSymbol, initType, line, column)) {
                return TYPE_UNDEFINED;
            }
            varSymbol->isInitialized = true;
        }
    }

    return TYPE_VOID;
}

void AssignNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "Assign ";
    if (!fieldName.empty()) {
        std::cout << varName << "." << fieldName;
    }
    else {
        std::cout << varName;
    }
    std::cout << " =\n";
    if (expression) {
        expression->print(indent + 2);
    }
}

DataType AssignNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    Symbol* leftSymbol = sem.findSymbol(varName);
    if (!leftSymbol) {
        sem.addError("Переменная '" + varName + "' не объявлена", line, column);
        return TYPE_UNDEFINED;
    }

    DataType exprType = TYPE_UNDEFINED;
    if (expression) {
        exprType = expression->checkSemantics(sem, currentSymbol);
    }

    if (exprType != TYPE_UNDEFINED) {
        if (!sem.checkAssignment(leftSymbol, exprType, line, column)) {
            return TYPE_UNDEFINED;
        }
        leftSymbol->isInitialized = true;
    }

    return exprType;
}

void ForLoopNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "For:\n";
    std::cout << spaces << "  Init:\n";
    if (init) init->print(indent + 4);
    std::cout << spaces << "  Condition:\n";
    if (condition) condition->print(indent + 4);
    std::cout << spaces << "  Increment:\n";
    if (increment) increment->print(indent + 4);
    std::cout << spaces << "  Body:\n";
    if (body) body->print(indent + 4);
}

DataType ForLoopNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    DataType initType = TYPE_UNDEFINED;
    DataType condType = TYPE_UNDEFINED;
    DataType incType = TYPE_UNDEFINED;

    if (init) initType = init->checkSemantics(sem, currentSymbol);
    if (condition) condType = condition->checkSemantics(sem, currentSymbol);
    if (increment) incType = increment->checkSemantics(sem, currentSymbol);

    if (!sem.checkForLoop(initType, condType, incType, line, column)) {
        return TYPE_UNDEFINED;
    }

    if (body) {
        sem.enterScope();
        body->checkSemantics(sem, currentSymbol);
        sem.leaveScope();
    }

    return TYPE_VOID;
}

void BinaryOpNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "BinaryOp ";
    // Простой способ получить имя операции
    switch (op) {
    case TK_PLUS: std::cout << "+"; break;
    case TK_MINUS: std::cout << "-"; break;
    case TK_MUL: std::cout << "*"; break;
    case TK_DIV: std::cout << "/"; break;
    case TK_MOD: std::cout << "%"; break;
    case TK_EQ: std::cout << "=="; break;
    case TK_NE: std::cout << "!="; break;
    case TK_LT: std::cout << "<"; break;
    case TK_LE: std::cout << "<="; break;
    case TK_GT: std::cout << ">"; break;
    case TK_GE: std::cout << ">="; break;
    case TK_BIT_AND: std::cout << "&"; break;
    case TK_BIT_OR: std::cout << "|"; break;
    case TK_BIT_XOR: std::cout << "^"; break;
    case TK_SHL: std::cout << "<<"; break;
    case TK_SHR: std::cout << ">>"; break;
    default: std::cout << "UNKNOWN"; break;
    }
    std::cout << ":\n";
    if (left) left->print(indent + 2);
    if (right) right->print(indent + 2);
}

DataType BinaryOpNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    DataType leftType = TYPE_UNDEFINED;
    DataType rightType = TYPE_UNDEFINED;

    if (left) leftType = left->checkSemantics(sem, currentSymbol);
    if (right) rightType = right->checkSemantics(sem, currentSymbol);

    if (leftType == TYPE_UNDEFINED || rightType == TYPE_UNDEFINED) {
        return TYPE_UNDEFINED;
    }

    return sem.checkBinaryOperation(op, leftType, rightType, line, column);
}

void UnaryOpNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "UnaryOp ";
    switch (op) {
    case TK_PLUS: std::cout << "+"; break;
    case TK_MINUS: std::cout << "-"; break;
    case TK_BIT_NOT: std::cout << "~"; break;
    default: std::cout << "UNKNOWN"; break;
    }
    std::cout << ":\n";
    if (operand) operand->print(indent + 2);
}

DataType UnaryOpNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    if (!operand) {
        return TYPE_UNDEFINED;
    }

    DataType operandType = operand->checkSemantics(sem, currentSymbol);

    if (operandType == TYPE_UNDEFINED) {
        return TYPE_UNDEFINED;
    }

    if (op == TK_PLUS || op == TK_MINUS) {
        if (!sem.isNumericType(operandType)) {
            sem.addError("Унарные + и - применимы только к числовым типам", line, column);
            return TYPE_UNDEFINED;
        }
        return operandType;
    }

    if (op == TK_BIT_NOT) {
        if (!sem.isIntegerType(operandType)) {
            sem.addError("Побитовое НЕ применимо только к целочисленным типам", line, column);
            return TYPE_UNDEFINED;
        }
        return operandType;
    }

    return TYPE_UNDEFINED;
}

void VarNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "Var ";
    if (!fieldName.empty()) {
        std::cout << name << "." << fieldName;
    }
    else {
        std::cout << name;
    }
    std::cout << "\n";
}

DataType VarNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    Symbol* symbol = sem.findSymbol(name);
    if (!symbol) {
        sem.addError("Идентификатор '" + name + "' не объявлен", line, column);
        return TYPE_UNDEFINED;
    }

    if (!fieldName.empty()) {
        DataType fieldType = TYPE_UNDEFINED;
        if (!sem.checkFieldAccess(symbol, fieldName, &fieldType, line, column)) {
            return TYPE_UNDEFINED;
        }
        nodeType = fieldType;
    }
    else {
        nodeType = symbol->type;
    }

    return nodeType;
}
void ConstNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "Const " << SemanticAnalyzer::dataTypeToString(type)
        << " " << value << "\n";
}

DataType ConstNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    return type;
}

void BlockNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "Block:\n";
    for (const auto& stmt : statements) {
        if (stmt) stmt->print(indent + 2);
    }
}

DataType BlockNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->checkSemantics(sem, currentSymbol);
        }
    }
    return TYPE_VOID;
}

void ReturnNode::print(int indent) const {
    std::string spaces(indent, ' ');
    std::cout << spaces << "Return";
    if (expression) {
        std::cout << ":\n";
        expression->print(indent + 2);
    }
    else {
        std::cout << "\n";
    }
}

DataType ReturnNode::checkSemantics(SemanticAnalyzer& sem, Symbol*& currentSymbol) {
    if (expression) {
        return expression->checkSemantics(sem, currentSymbol);
    }
    return TYPE_VOID;
}

void Parser::printAST(const ASTNode* node) {
    if (!node) {
        std::cout << "AST is empty\n";
        return;
    }
    node->print();
}