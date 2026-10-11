#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>
class decoder {
    int date;
    enum class mode {
        normalReading = 1,
        valueReading
    };

  public:
    void decode();
    int decode_int(mode &currentMode, std::ifstream &torrentFile);
    std::string decode_string(mode &currentMode, std::ifstream &torrentFile);
};

void decoder::decode() {
    mode currentMode = mode::normalReading;
    std::stack<char> typeStack;
    std::ifstream torrentFile("torex.torrent");
    if (!torrentFile.is_open()) {
        std::cout << "Error reading the file\n";
        return;
    }

    char character;
    while (torrentFile.get(character)) {
        if (character == 'i' && currentMode == mode::normalReading) {
            typeStack.push('i');
            int decodedIntValue = decode_int(currentMode, torrentFile);
            std::cout << decodedIntValue;
            typeStack.pop();
        }
        if (std::isdigit(static_cast<unsigned char>(character)) && currentMode == mode::normalReading) {
            std::string decodedString = decode_string(currentMode, torrentFile);
            std::cout << decodedString;
        }
    }
}
int decoder::decode_int(mode &currentMode, std::ifstream &torrentFile) {
    currentMode = mode::valueReading;
    std::string tempArray;
    int realInteger = 0;
    char character;
    while (torrentFile.get(character)) {
        if (character == 'e') {
            break;
        }
        tempArray += character;
    }
    realInteger = std::stoi(tempArray);
    currentMode = mode::normalReading;
    return realInteger;
}
std::string decoder::decode_string(mode &currentMode, std::ifstream &torrentFile) {
    torrentFile.unget();
    currentMode = mode::valueReading;
    std::string tempNumber;
    char character;
    while (torrentFile.get(character)) {
        if (character == ':') {
            break;
        }
        tempNumber += character;
    }

    int stringLength = std::stoi(tempNumber);
    std::string decoded_string;
    for (int i = 0; i < stringLength; i++) {
        torrentFile.get(character);
        decoded_string += character;
    }
    currentMode = mode::normalReading;
    return decoded_string;
}
int main() {
    decoder harmoniaDecoder;
    harmoniaDecoder.decode();
}
