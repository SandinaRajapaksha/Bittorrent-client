#include <fstream>
#include <iostream>
#include <stack>
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
            currentMode = mode::valueReading;
            typeStack.push('i');
            int decodedIntValue = decode_int(currentMode, torrentFile);
            std::cout << decodedIntValue;
            typeStack.pop();
        }
    }
}
int decoder::decode_int(mode &currentMode, std::ifstream &torrentFile) {
    std::string tempArray;
    int realInteger = 0;
    char character;
    int i = 0;
    while (torrentFile.get(character)) {
        if (character == 'e') {
            break;
        }
        tempArray += character;
        i++;
    }
    realInteger = std::stoi(tempArray);
    currentMode = mode::normalReading;
    return realInteger;
}
int main() {
    decoder harmoniaDecoder;
    harmoniaDecoder.decode();
}
