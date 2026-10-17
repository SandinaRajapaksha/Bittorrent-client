#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
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

class decoder {
  private:
    int listCounter;
    std::stack<char> typeStack;
    // whole data structure
    std::map<std::string, BValue> root;

  public:
    decoder() {
        listCounter = 0;
    }
    void decode();
    // base types
    uint64_t decode_int(std::ifstream &torrentFile);
    std::string decode_string(std::ifstream &torrentFile);
    void decode_list(std::ifstream &torrentFile);
};

void decoder::decode() {
    if (!torrentFile.is_open()) {
        std::cout << "Error reading the file\n";
        return;
    }

    char character;
    while (torrentFile.get(character)) {
        if (character == 'l') {
            decode_list(torrentFile);
        }
        if (character == 'i') {
            typeStack.push('i');
            static uint64_t decodedIntValue = decode_int(torrentFile);
            std::cout << decodedIntValue;
            typeStack.pop();
        }
        if (std::isdigit(static_cast<unsigned char>(character))) {
            static std::string decodedString = decode_string(torrentFile);
            std::cout << decodedString;
        }
    }
}
uint64_t decoder::decode_int(std::ifstream &torrentFile) {
    std::string tempArray;
    uint64_t realInteger = 0;
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
std::string decoder::decode_string(std::ifstream &torrentFile) {
    torrentFile.unget();
    std::string tempNumber;
    char character;
    while (torrentFile.get(character)) {
        if (character == ':') {
            break;
        }
        tempNumber += character;
    }

    uint64_t stringLength = std::stoi(tempNumber);
    std::string decoded_string;
    for (int i = 0; i < stringLength; i++) {
        torrentFile.get(character);
        decoded_string += character;
    }
    return decoded_string;
}

void decoder::decode_list(std::ifstream &torrentFile) {
    // divide
    typeStack.push('l');
    listCounter++;
    char character;
    torrentFile.get(character);
    if (character == 'l') {
        decode_list(torrentFile);
        return;
    }

    // array of ints
    std::vector<uint64_t> baseVector;

    while (!(character == 'e' && typeStack.top() == 'l')) {
        if (character == 'i') {
            uint64_t value = decode_int(torrentFile);
            typeStack.push('i');
            torrentFile.unget();
            torrentFile.get(character);
            if (character == 'e' && typeStack.top() == 'i') {
                typeStack.pop();
            }
            baseVector.push_back(value);
        }
    }
    for (const auto &element : baseVector) {
        std::cout << element << "-";
    }
}

int main() {
    decoder harmoniaDecoder;
    harmoniaDecoder.decode();
}
