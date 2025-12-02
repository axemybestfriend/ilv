#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

// Типы токенов
enum TokenType {
    // Ключевые слова
    TK_INT = 100, TK_SHORT, TK_LONG, TK_FLOAT,
    TK_STRUCT, TK_FOR, TK_RETURN, TK_VOID,

    // Операторы
    TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_MOD,
    TK_ASSIGN, TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE,
    TK_SEMICOLON, TK_COMMA, TK_DOT, TK_COLON,

    // Операции сравнения
    TK_EQ, TK_NE, TK_LT, TK_LE, TK_GT, TK_GE,

    // Побитовые операции
    TK_BIT_AND, TK_BIT_OR, TK_BIT_XOR, TK_BIT_NOT,
    TK_SHL, TK_SHR,

    // Идентификаторы и константы
    TK_IDENT, TK_INT_CONST, TK_FLOAT_CONST,

    // Специальные токены
    TK_EOF, TK_ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType t = TK_ERROR, std::string l = "", int ln = 0, int col = 0)
        : type(t), lexeme(l), line(ln), column(col) {}

    std::string typeToString() const;
};

class Scanner {
private:

   

    static std::unordered_map<std::string, TokenType> keywords;

    char getChar();
    char peekChar();
    void skipWhitespace();
    void skipComment();

    Token scanNumber();
    Token scanIdentifier();
    Token scanOperator();

public:

    Scanner(const std::string& filename);
    ~Scanner();

    std::ifstream file;
    int line;
    int column;
    char currentChar;
    bool eof;

    bool open();
    Token getNextToken();
    void reset();


    std::streampos getPosition(){
        return file.tellg();
    }

    void setPosition(std::streampos pos) {
        file.seekg(pos);
        eof = false;
    }

    Token peekNextToken();
};