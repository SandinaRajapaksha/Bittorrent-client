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

class decoder {
  private:
    std::stack<char> typeStack;
    // whole data structure
    std::map<std::string, BValue> root;

  public:
    void decode();
    // base types
    BValue decode_int(std::ifstream &torrentFile);
    std::string decode_string(std::ifstream &torrentFile);
    std::vector<std::variant<int64_t, std::string>> decode_list(std::ifstream &torrentFile);
};

void decoder::decode() {
    if (!torrentFile.is_open()) {
        std::cout << "Error reading the file\n";
        return;
    }

    char character;
    while (torrentFile.get(character)) {
        if (character == 'l') {
            std::vector<std::variant<int64_t, std::string>> baseVector = decode_list(torrentFile);

            // just printing stuff
            // single loop, by const reference
            for (const auto &elem : baseVector) {
                std::visit([](const auto &val) {
                    std::cout << val;
                },
                           elem);
            }

        } else if (character == 'i') {
            BValue decodedIntValue = decode_int(torrentFile);
            std::cout << decodedIntValue.asInt();
            // std::cout << decodedIntValue;
        } else if (std::isdigit(static_cast<unsigned char>(character))) {
            std::string decodedString = decode_string(torrentFile);
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

    int64_t stringLength = std::stoi(tempNumber);
    std::string decoded_string;
    for (int i = 0; i < stringLength; i++) {
        torrentFile.get(character);
        decoded_string += character;
    }
    return decoded_string;
}

std::vector<std::variant<int64_t, std::string>> decoder::decode_list(std::ifstream &torrentFile) {
    char character;
    torrentFile.unget();
    torrentFile.get(character);
    if (character == 'l') {
        std::vector<std::variant<int64_t, std::string>> atomicListValue = decode_list(torrentFile);
    }

    std::vector<std::variant<int64_t, std::string>> baseVector;
    while (true) {
        if (character == 'i') {
            BValue value = decode_int(torrentFile);
            torrentFile.unget();
            torrentFile.get(character);
            // baseVector.push_back(value);

        } else if (std::isdigit(static_cast<unsigned char>(character))) {
            std::string value = decode_string(torrentFile);
            torrentFile.unget();
            torrentFile.get(character);

            baseVector.push_back(value);
        }
        if (character == 'e') {
            break;
        }
        torrentFile.get(character);
    }
    return baseVector;
}
int main() {
    decoder harmoniaDecoder;
    harmoniaDecoder.decode();
}
