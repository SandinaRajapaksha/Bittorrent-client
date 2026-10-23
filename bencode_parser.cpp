#include "bencode_parser.h"
#include <fstream>

static std::ifstream torrentFile("bigBuck.torrent");
void printBValue(const BValue &v, int indent = 0) {
    std::string pad(indent, ' ');

    if (v.isInt()) {
        std::cout << pad << v.asInt() << "\n";
    } else if (v.isString()) {
        const std::string &s = v.asString();
        if (s.size() > 60) {
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
            BValue decodedList = decode_list(torrentFile);
        } else if (character == 'i') {
            BValue decodedIntValue = decode_int(torrentFile);
        } else if (std::isdigit(static_cast<unsigned char>(character))) {
            BValue decodedString = decode_string(torrentFile);
        } else if (character == 'd') {
            BValue decodedDict = decode_dictionary(torrentFile);
            printBValue(decodedDict);
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
    realInteger = std::stoll(tempArray);
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

    int64_t stringLength = std::stoll(tempNumber);
    std::string decoded_string;
    for (int i = 0; i < stringLength; i++) {
        torrentFile.get(character);
        decoded_string += character;
    }
    BValue decoded_str(decoded_string);
    return decoded_str;
}

BValue decoder::decode_list(std::ifstream &torrentFile) {
    std::vector<BValue> list;
    char character;

    while (torrentFile.get(character)) {
        if (character == 'e') {
            break;
        }
        if (character == 'l') {
            list.push_back(decode_list(torrentFile));
        } else if (character == 'i') {
            list.push_back(decode_int(torrentFile));
        } else if (std::isdigit(static_cast<unsigned char>(character))) {
            list.push_back(decode_string(torrentFile));
        } else if (character == 'd') {
            list.push_back(decode_dictionary(torrentFile));
        }
    }
    return BValue(list);
}

BValue decoder::decode_dictionary(std::ifstream &torrentFile) {
    std::map<std::string, BValue> decodedDictionary;
    char character;

    while (torrentFile.get(character)) {
        if (character == 'e') {
            break;
        }

        if (std::isdigit(static_cast<unsigned char>(character))) {
            BValue keyValue = decode_string(torrentFile);
            std::string key = keyValue.asString();

            if (!torrentFile.get(character)) {
                break;
            }

            if (character == 'd') {
                BValue value = decode_dictionary(torrentFile);
                decodedDictionary[key] = value;
            } else if (character == 'l') {
                BValue value = decode_list(torrentFile);
                decodedDictionary[key] = value;
            } else if (character == 'i') {
                BValue value = decode_int(torrentFile);
                decodedDictionary[key] = value;
            } else if (std::isdigit(static_cast<unsigned char>(character))) {
                BValue value = decode_string(torrentFile);
                decodedDictionary[key] = value;
            }
        }
    }
    return decodedDictionary;
};
