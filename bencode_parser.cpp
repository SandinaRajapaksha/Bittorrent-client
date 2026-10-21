#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <ostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <variant>
#include <vector>

static std::ifstream torrentFile("torex.torrent");
class BValue {
  public:
    using Int = int64_t;
    using Str = std::string;
    using List = std::vector<BValue>;
    using Dict = std::map<std::string, BValue>;

    // Constructors: one per kind of data
    BValue(Int i) : data_(i) {}
    BValue(Str s) : data_(std::move(s)) {}
    BValue(List l) : data_(std::move(l)) {}
    BValue(Dict d) : data_(std::move(d)) {}

    // Type checks
    bool isInt() const { return std::holds_alternative<Int>(data_); }
    bool isString() const { return std::holds_alternative<Str>(data_); }
    bool isList() const { return std::holds_alternative<List>(data_); }
    bool isDict() const { return std::holds_alternative<Dict>(data_); }

    // Getters (throw if you ask for the wrong type)
    Int asInt() const { return get<Int>("integer"); }
    const Str &asString() const { return get<Str>("string"); }
    const List &asList() const { return get<List>("list"); }
    const Dict &asDict() const { return get<Dict>("dictionary"); }

  private:
    template <typename T>
    const T &get(const char *name) const {
        const T *p = std::get_if<T>(&data_);
        if (!p)
            throw std::runtime_error(std::string("BValue is not a ") + name);
        return *p;
    }

    std::variant<Int, Str, List, Dict> data_;
};

std::vector<BValue> parsedList;
class decoder {
  private:
    std::stack<char> typeStack;
    // whole data structure
    std::map<std::string, BValue> root;

  public:
    void decode();
    // base types
    BValue decode_int(std::ifstream &torrentFile);
    BValue decode_string(std::ifstream &torrentFile);
    BValue decode_list(std::ifstream &torrentFile);
    BValue decode_dictionary(std::ifstream &torrentFile);
};

void printBValue(const BValue &v, int indent = 0) {
    std::string pad(indent, ' ');

    if (v.isInt()) {
        std::cout << pad << v.asInt() << "\n";
    } else if (v.isString()) {
        const std::string &s = v.asString();
        if (s.size() > 60) {
            // long strings are probably binary (piece hashes), so just show the length
            std::cout << pad << "<" << s.size() << " bytes>\n";
        } else {
            std::cout << pad << s << "\n";
        }
    } else if (v.isList()) {
        std::cout << pad << "[\n";
        for (const auto &item : v.asList()) {
            printBValue(item, indent + 2);
        }
        std::cout << pad << "]\n";
    } else if (v.isDict()) {
        std::cout << pad << "{\n";
        for (const auto &[key, value] : v.asDict()) {
            std::cout << pad << "  " << key << ":\n";
            printBValue(value, indent + 4);
        }
        std::cout << pad << "}\n";
    }
}

void decoder::decode() {
    if (!torrentFile.is_open()) {
        std::cout << "Error reading the file\n";
        return;
    }

    char character;
    while (torrentFile.get(character)) {
        if (character == 'l') {
            // flushing out current list
            std::vector<BValue> newPlaceholder;
            newPlaceholder.clear();
            parsedList = newPlaceholder;

            decode_list(torrentFile);
            BValue decodedList = parsedList;

            // just printing stuff
            printBValue(decodedList);

        } else if (character == 'i') {
            BValue decodedIntValue = decode_int(torrentFile);
            std::cout << decodedIntValue.asInt();
        } else if (std::isdigit(static_cast<unsigned char>(character))) {
            BValue decodedString = decode_string(torrentFile);
            // std::cout << decodedString;
        }
    }
}
BValue decoder::decode_int(std::ifstream &torrentFile) {
    std::string tempArray;
    BValue realInteger = 0;
    char character;
    while (torrentFile.get(character)) {
        if (character == 'e') {
            break;
        }
        tempArray += character;
    }
    realInteger = std::stoi(tempArray);
    return realInteger;
}
BValue decoder::decode_string(std::ifstream &torrentFile) {
    torrentFile.unget();
    std::string tempNumber;
    char character;
    while (torrentFile.get(character)) {
        if (character == ':') {
            break;
        }
        tempNumber += character;
    }

    int64_t stringLength = std::stoi(tempNumber);
    std::string decoded_string;
    for (int i = 0; i < stringLength; i++) {
        torrentFile.get(character);
        decoded_string += character;
    }
    BValue decoded_str(decoded_string);
    return decoded_str;
}

BValue decoder::decode_list(std::ifstream &torrentFile) {
    char character;
    torrentFile.get(character);
    while (character == 'l') {
        parsedList.push_back(decode_list(torrentFile));
        torrentFile.get(character);
        return 0;
    }

    std::vector<BValue> atomicList;
    if (character == 'i') {
        while (character != 'e') {
            BValue value = decode_int(torrentFile);
            atomicList.push_back(value);
            torrentFile.get(character);
        }
    }
    if (std::isdigit(static_cast<unsigned char>(character))) {
        while (character != 'e') {
            BValue value = decode_string(torrentFile);
            atomicList.push_back(value);
            torrentFile.get(character);
        }
    }
    return atomicList;
}
int main() {
    decoder harmoniaDecoder;
    harmoniaDecoder.decode();
}
