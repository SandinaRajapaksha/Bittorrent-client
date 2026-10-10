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
    void decode_int(mode &currentMode, std::ifstream &torrentFile);
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
        std::cout << character;
        if (character == 'i' && currentMode == mode::normalReading) {
            typeStack.push('i');
            decode_int(currentMode, torrentFile);
            typeStack.pop();
        }
    }
}
void decoder::decode_int(mode &currentMode, std::ifstream &torrentFile) {
    std::vector<int> tempArray;
    int realInteger;
    char character;
    int i = 0;
    while (torrentFile.get(character)) {
        if (character == 'e') {
            break;
        }
        tempArray[i] = (int)character;
        i++;
    }
}
int main() {
}
