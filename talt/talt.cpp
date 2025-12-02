#include <iostream>
#include <fstream>
#include <sstream>
#include "scanner.h"
#include "parser.h"
#include "semantic.h"

void printToken(const Token& token) {
    std::cout << "[" << token.line << ":" << token.column << "] "
        << token.typeToString() << " '" << token.lexeme << "'\n";
}

void testScanner(const std::string& filename) {
    std::cout << "\n=== ТЕСТИРОВАНИЕ СКАНЕРА ===" << std::endl;

    Scanner scanner(filename);
    if (!scanner.open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return;
    }

    Token token;
    int tokenCount = 0;

    do {
        token = scanner.getNextToken();
        printToken(token);
        tokenCount++;
    } while (token.type != TK_EOF && token.type != TK_ERROR);

    std::cout << "Всего токенов: " << tokenCount << std::endl;
}

void testParser(const std::string& filename) {
    std::cout << "\n=== ТЕСТИРОВАНИЕ ПАРСЕРА И СЕМАНТИЧЕСКОГО АНАЛИЗА ===" << std::endl;

    Scanner scanner(filename);
    if (!scanner.open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return;
    }

    SemanticAnalyzer semantic;
    Parser parser(scanner, semantic);

    auto ast = parser.parse();

    if (parser.hasError) {
        std::cout << "В процессе разбора возникли ошибки." << std::endl;
    }

    if (ast) {
        std::cout << "\n=== АБСТРАКТНОЕ СИНТАКСИЧЕСКОЕ ДЕРЕВО ===" << std::endl;
        parser.printAST(ast.get());

        std::cout << "\n=== СЕМАНТИЧЕСКИЙ АНАЛИЗ ===" << std::endl;
        Symbol* dummy = nullptr;
        ast->checkSemantics(semantic, dummy);

        semantic.printStructTypes();
        semantic.printSymbolTable();
        semantic.printErrors();
        semantic.printWarnings();

        if (!semantic.hasErrors()) {
            std::cout << "\n✓ Программа корректна" << std::endl;
        }
        else {
            std::cout << "\n✗ Обнаружены ошибки" << std::endl;
        }
    }
    else {
        std::cout << "Не удалось построить AST." << std::endl;
    }
}

void processFile(const std::string& filename) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ОБРАБОТКА ФАЙЛА: " << filename << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    // Тестируем сканер
    testScanner(filename);

    // Тестируем парсер и семантический анализ
    testParser(filename);
}

int main() {
    setlocale(LC_ALL, "rus");
    // Запускаем все тесты
    processFile("test_correct.txt");
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "\n";

    processFile("test_error1.txt");
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "\n";

    processFile("test_error2.txt");

    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ВСЕ ТЕСТЫ ВЫПОЛНЕНЫ" << std::endl;

    // Пауза
    std::cout << "\nНажмите Enter для выхода...";
    std::cin.get();

    return 0;
}