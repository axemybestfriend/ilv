#include "scanner.h"
#include <cctype>
#include <iostream>
#include <sstream>

std::unordered_map<std::string, TokenType> Scanner::keywords = {
    {"int", TK_INT}, {"short", TK_SHORT}, {"long", TK_LONG},
    {"float", TK_FLOAT}, {"struct", TK_STRUCT}, {"for", TK_FOR},
    {"return", TK_RETURN}, {"void", TK_VOID}
};

std::string Token::typeToString() const {
    static std::unordered_map<TokenType, std::string> tokenNames = {
        {TK_INT, "TK_INT"}, {TK_SHORT, "TK_SHORT"}, {TK_LONG, "TK_LONG"},
        {TK_FLOAT, "TK_FLOAT"}, {TK_STRUCT, "TK_STRUCT"}, {TK_FOR, "TK_FOR"},
        {TK_RETURN, "TK_RETURN"}, {TK_VOID, "TK_VOID"},
        {TK_PLUS, "TK_PLUS"}, {TK_MINUS, "TK_MINUS"}, {TK_MUL, "TK_MUL"},
        {TK_DIV, "TK_DIV"}, {TK_MOD, "TK_MOD"}, {TK_ASSIGN, "TK_ASSIGN"},
        {TK_LPAREN, "TK_LPAREN"}, {TK_RPAREN, "TK_RPAREN"},
        {TK_LBRACE, "TK_LBRACE"}, {TK_RBRACE, "TK_RBRACE"},
        {TK_SEMICOLON, "TK_SEMICOLON"}, {TK_COMMA, "TK_COMMA"},
        {TK_DOT, "TK_DOT"}, {TK_COLON, "TK_COLON"},
        {TK_EQ, "TK_EQ"}, {TK_NE, "TK_NE"}, {TK_LT, "TK_LT"},
        {TK_LE, "TK_LE"}, {TK_GT, "TK_GT"}, {TK_GE, "TK_GE"},
        {TK_BIT_AND, "TK_BIT_AND"}, {TK_BIT_OR, "TK_BIT_OR"},
        {TK_BIT_XOR, "TK_BIT_XOR"}, {TK_BIT_NOT, "TK_BIT_NOT"},
        {TK_SHL, "TK_SHL"}, {TK_SHR, "TK_SHR"},
        {TK_IDENT, "TK_IDENT"}, {TK_INT_CONST, "TK_INT_CONST"},
        {TK_FLOAT_CONST, "TK_FLOAT_CONST"},
        {TK_EOF, "TK_EOF"}, {TK_ERROR, "TK_ERROR"}
    };

    auto it = tokenNames.find(type);
    if (it != tokenNames.end()) return it->second;
    return "UNKNOWN_TOKEN";
}

Scanner::Scanner(const std::string& filename)
    : line(1), column(0), eof(false), currentChar(' ') {
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
    }
}

Scanner::~Scanner() {
    if (file.is_open()) file.close();
}

bool Scanner::open() {
    return file.is_open();
}

char Scanner::getChar() {
    if (!file.get(currentChar)) {
        eof = true;
        currentChar = '\0';
    }
    else {
        column++;
        if (currentChar == '\n') {
            line++;
            column = 0;
        }
    }
    return currentChar;
}

char Scanner::peekChar() {
    if (eof) return '\0';
    char ch = file.peek();
    return ch;
}

void Scanner::skipWhitespace() {
    while (std::isspace(currentChar) && !eof) {
        getChar();
    }
}

void Scanner::skipComment() {
    if (currentChar == '/' && peekChar() == '/') {
        while (currentChar != '\n' && !eof) {
            getChar();
        }
        if (!eof) getChar(); // съедаем \n
    }
    else if (currentChar == '/' && peekChar() == '*') {
        getChar(); // съедаем *
        getChar(); // съедаем следующий символ
        while (!eof) {
            if (currentChar == '*' && peekChar() == '/') {
                getChar(); // съедаем *
                getChar(); // съедаем /
                break;
            }
            getChar();
        }
    }
}

Token Scanner::scanNumber() {
    std::string num;
    int startLine = line;
    int startCol = column;
    bool isFloat = false;
    bool hasExponent = false;

    // Целая часть
    while (std::isdigit(currentChar) && !eof) {
        num += currentChar;
        getChar();
    }

    // Дробная часть
    if (currentChar == '.') {
        isFloat = true;
        num += currentChar;
        getChar();

        while (std::isdigit(currentChar) && !eof) {
            num += currentChar;
            getChar();
        }
    }

    // Экспоненциальная часть
    if (currentChar == 'e' || currentChar == 'E') {
        isFloat = true;
        hasExponent = true;
        num += currentChar;
        getChar();

        if (currentChar == '+' || currentChar == '-') {
            num += currentChar;
            getChar();
        }

        if (!std::isdigit(currentChar)) {
            return Token(TK_ERROR, num, startLine, startCol);
        }

        while (std::isdigit(currentChar) && !eof) {
            num += currentChar;
            getChar();
        }
    }

    // Проверяем, что следующий символ не буква (чтобы не было 123abc)
    if (std::isalpha(currentChar) || currentChar == '_') {
        while (std::isalnum(currentChar) || currentChar == '_') {
            num += currentChar;
            getChar();
        }
        return Token(TK_ERROR, num, startLine, startCol);
    }

    return Token(isFloat ? TK_FLOAT_CONST : TK_INT_CONST,
        num, startLine, startCol);
}

Token Scanner::scanIdentifier() {
    std::string id;
    int startLine = line;
    int startCol = column;

    while ((std::isalnum(currentChar) || currentChar == '_') && !eof) {
        id += currentChar;
        getChar();
    }

    auto it = keywords.find(id);
    if (it != keywords.end()) {
        return Token(it->second, id, startLine, startCol);
    }

    return Token(TK_IDENT, id, startLine, startCol);
}

Token Scanner::scanOperator() {
    int startLine = line;
    int startCol = column;
    char firstChar = currentChar;
    std::string op(1, firstChar);

    getChar();

    switch (firstChar) {
    case '+': return Token(TK_PLUS, op, startLine, startCol);
    case '-': return Token(TK_MINUS, op, startLine, startCol);
    case '*': return Token(TK_MUL, op, startLine, startCol);
    case '/': return Token(TK_DIV, op, startLine, startCol);
    case '%': return Token(TK_MOD, op, startLine, startCol);
    case '=':
        if (currentChar == '=') {
            getChar();
            return Token(TK_EQ, "==", startLine, startCol);
        }
        return Token(TK_ASSIGN, "=", startLine, startCol);
    case '<':
        if (currentChar == '=') {
            getChar();
            return Token(TK_LE, "<=", startLine, startCol);
        }
        else if (currentChar == '<') {
            getChar();
            return Token(TK_SHL, "<<", startLine, startCol);
        }
        return Token(TK_LT, "<", startLine, startCol);
    case '>':
        if (currentChar == '=') {
            getChar();
            return Token(TK_GE, ">=", startLine, startCol);
        }
        else if (currentChar == '>') {
            getChar();
            return Token(TK_SHR, ">>", startLine, startCol);
        }
        return Token(TK_GT, ">", startLine, startCol);
    case '!':
        if (currentChar == '=') {
            getChar();
            return Token(TK_NE, "!=", startLine, startCol);
        }
        break;
    case '&':
        return Token(TK_BIT_AND, "&", startLine, startCol);
    case '|':
        return Token(TK_BIT_OR, "|", startLine, startCol);
    case '^':
        return Token(TK_BIT_XOR, "^", startLine, startCol);
    case '~':
        return Token(TK_BIT_NOT, "~", startLine, startCol);
    case '(': return Token(TK_LPAREN, "(", startLine, startCol);
    case ')': return Token(TK_RPAREN, ")", startLine, startCol);
    case '{': return Token(TK_LBRACE, "{", startLine, startCol);
    case '}': return Token(TK_RBRACE, "}", startLine, startCol);
    case ';': return Token(TK_SEMICOLON, ";", startLine, startCol);
    case ',': return Token(TK_COMMA, ",", startLine, startCol);
    case '.': return Token(TK_DOT, ".", startLine, startCol);
    case ':': return Token(TK_COLON, ":", startLine, startCol);
    }

    return Token(TK_ERROR, std::string(1, firstChar), startLine, startCol);
}

Token Scanner::getNextToken() {
    if (eof) return Token(TK_EOF, "", line, column);

    skipWhitespace();

    // Проверка на комментарии
    if (currentChar == '/' && (peekChar() == '/' || peekChar() == '*')) {
        skipComment();
        skipWhitespace();
    }

    if (eof) return Token(TK_EOF, "", line, column);

    if (std::isdigit(currentChar)) {
        return scanNumber();
    }
    else if (std::isalpha(currentChar) || currentChar == '_') {
        return scanIdentifier();
    }
    else if (std::ispunct(currentChar)) {
        return scanOperator();
    }
    else if (currentChar == '\0') {
        return Token(TK_EOF, "", line, column);
    }

    Token error(TK_ERROR, std::string(1, currentChar), line, column);
    getChar();
    return error;
}

void Scanner::reset() {
    if (file.is_open()) {
        file.clear();
        file.seekg(0);
    }
    line = 1;
    column = 0;
    eof = false;
    currentChar = ' ';
}

Token Scanner::peekNextToken() {
    std::streampos oldPos = file.tellg();
    int oldLine = line;
    int oldCol = column;
    char oldChar = currentChar;
    bool oldEof = eof;

    Token next = getNextToken();

    file.seekg(oldPos);
    line = oldLine;
    column = oldCol;
    currentChar = oldChar;
    eof = oldEof;

    return next;
}