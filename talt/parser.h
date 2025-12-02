#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "semantic.h"
#include <memory>
#include <vector>
#include <string>

// Базовый класс узла AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
    virtual DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) = 0;
    virtual DataType getDataType() const { return TYPE_UNDEFINED; }
    virtual std::string getStringValue() const { return ""; }

    int line = 0;
    int column = 0;
};

// Конкретные классы узлов

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> declarations;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};

class StructDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<std::pair<std::string, DataType>> fields;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};

class FunctionNode : public ASTNode {
public:
    std::string name;
    DataType returnType;
    std::unique_ptr<ASTNode> body;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};


class VarDeclNode : public ASTNode {
public:
    std::string name;
    DataType type;
    std::string structName;  // Имя структуры, если type == TYPE_STRUCT
    std::unique_ptr<ASTNode> initValue;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};

class AssignNode : public ASTNode {
public:
    std::string varName;
    std::string fieldName;
    std::unique_ptr<ASTNode> expression;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};

class ForLoopNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};

class BinaryOpNode : public ASTNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};

class UnaryOpNode : public ASTNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> operand;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};

class VarNode : public ASTNode {
public:
    std::string name;
    std::string fieldName;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
    DataType getDataType() const override { return nodeType; }

private:
    DataType nodeType = TYPE_UNDEFINED;
    friend class Parser;
};

class ConstNode : public ASTNode {
public:
    DataType type;
    std::string value;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
    DataType getDataType() const override { return type; }
    std::string getStringValue() const override { return value; }
};

class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};

class ReturnNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;

    void print(int indent = 0) const override;
    DataType checkSemantics(SemanticAnalyzer& sem,
        Symbol*& currentSymbol) override;
};

class Parser {
private:
    Scanner& scanner;
    Token currentToken;
    SemanticAnalyzer& semantic;

    void advance();
    bool match(TokenType expected);
    bool check(TokenType expected) const;
    void error(const std::string& message);
    Token peekToken();

    // Правила грамматики
    std::unique_ptr<ProgramNode> parseProgram();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<StructDeclNode> parseStructDeclaration();
    std::unique_ptr<FunctionNode> parseFunctionDeclaration(DataType returnType);
    std::unique_ptr<VarDeclNode> parseVariableDeclaration(DataType type, const std::string& structTypeName = "");
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ForLoopNode> parseForLoop();
    std::unique_ptr<ReturnNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseBitwiseOr();
    std::unique_ptr<ASTNode> parseBitwiseXor();
    std::unique_ptr<ASTNode> parseBitwiseAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseRelational();
    std::unique_ptr<ASTNode> parseShift();
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseMultiplicative();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parsePostfix();
    std::unique_ptr<ASTNode> parsePrimary();

    DataType parseType(std::string* structTypeName = nullptr);
    std::unique_ptr<BlockNode> parseBlock();

public:
    Parser(Scanner& sc, SemanticAnalyzer& sem);

    std::unique_ptr<ProgramNode> parse();
    void printAST(const ASTNode* node);

    bool hasError = false;
};

#endif