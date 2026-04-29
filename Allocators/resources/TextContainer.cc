#include "TextContainer.hh"

TextContainer::TextContainer(char* const Buffer, bool (*isDelim)(const char c)) noexcept
    : start(Buffer),
      end(tokenize(Buffer, isDelim)) {
    // не константное поле можно инициализировать пост-фактум, независимо от определения.
    iter = start;
}

// Вместо аргумента по умолчанию я перегрузил конструктор. И использовал паттерн delegating constructor ради флекса.
TextContainer::TextContainer(char* const Buffer) noexcept
    : TextContainer(Buffer, default_delimeters) {
}

// Конструктор копирования для создания контейнеров с тем же текстом (на том же буфере) и со сброшенным итератором слов.
// Используется при передаче экземпляра класса как аргумента функции, внутри которой будет уже копия.
TextContainer::TextContainer(TextContainer& src) noexcept
    : start(src.start),
      end(src.end) {
    iter = src.start;
}

const char* const TextContainer::GetNextWord() {
    while (iter != end && *iter == '\0') {
        iter++;
    }
    // В начале слова или конеце буфера

    if (iter == end) {
        // Конец буфера
        return nullptr;
    }

    const char* const word = iter;
    while (*iter != '\0' && iter != end) {
        iter++;
    }
    // В конце слова или конеце буфера
    // anyway возвращаем найденное слово
    return word;
}

// Эта функция может выглядить громоздко, но состоит всего из 1~2 операций и многократно вызывается в tokenize - отличный кандидат для inline
// Однако вписывать такую колбасу в хедер - стрёмно. Но это и не нужно!
// Определение inline функций должно быть в каждй единице трансляции, использующей её.
// default_delimeters помечена как private и используется только, здесь, в файле, в котором её определение присутствует.
// Под словом использование, однако, подразумевается не только вызовы, но и передачи ссылки на функцию.
// Потому пришлось убрать её из умолчания аргумента в конструкторе.
// Конструктор вызываясь в main.cс и использовал ссылку на default_delimeters, несмотря на то, что прописаны умолчания были в этом файле.
inline bool TextContainer::default_delimeters(const char c) noexcept {
    switch (c) {
    case ' ':
    case '\n':
    case '\r':
    case '.':
    case ',':
    case '!':
    case '-':
    case ';':
    case ':':
    case '?':
    case '"':
    case '\'':
    case '(':
    case ')':
    case '[':
    case ']':
    case '/':
        return true;
    default:
        return false;
    }
}

bool TextContainer::utf8_delimiter(const char* const Buffer, size_t* const length) noexcept {
    const unsigned char first = static_cast<unsigned char>(Buffer[0]);
    *length = 1;

    if (first < 0x80) {
        return false;
    }

    size_t utf8_length = 0;
    unsigned int code_point = 0;
    if ((first & 0xE0) == 0xC0) {
        utf8_length = 2;
        code_point = first & 0x1F;
    } else if ((first & 0xF0) == 0xE0) {
        utf8_length = 3;
        code_point = first & 0x0F;
    } else if ((first & 0xF8) == 0xF0) {
        utf8_length = 4;
        code_point = first & 0x07;
    } else {
        return true;
    }

    for (size_t i = 1; i < utf8_length; ++i) {
        const unsigned char current = static_cast<unsigned char>(Buffer[i]);
        if ((current & 0xC0) != 0x80) {
            return true;
        }
        code_point = (code_point << 6) | (current & 0x3F);
    }

    if ((utf8_length == 2 && code_point < 0x80) ||
        (utf8_length == 3 && code_point < 0x800) ||
        (utf8_length == 4 && (code_point < 0x10000 || code_point > 0x10FFFF)) ||
        (code_point >= 0xD800 && code_point <= 0xDFFF)) {
        return true;
    }

    *length = utf8_length;

    return code_point == 0x00A0 ||  // non-breaking space
           code_point == 0x00AB ||  // «
           code_point == 0x00BB ||  // »
           code_point == 0xFEFF ||  // UTF-8 byte order mark
           (code_point >= 0x2000 && code_point <= 0x206F);
}

char* const TextContainer::tokenize(char* const Buffer, bool (*isDelimeter)(const char c)) noexcept {
    char* iter = Buffer;
    while (*iter != '\0') {
        size_t delimiter_length = 1;
        if (utf8_delimiter(iter, &delimiter_length)) {
            for (size_t i = 0; i < delimiter_length; ++i) {
                iter[i] = '\0';
            }
            iter += delimiter_length;
            continue;
        }
        if (delimiter_length > 1) {
            iter += delimiter_length;
            continue;
        }
        if (isDelimeter(*iter)) {
            *iter = '\0';
        }
        ++iter;
    }
    return iter;  // Указывает на конец буфера
}
