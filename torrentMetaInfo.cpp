#include "torrentMetaInfo.h"
#include "commonheader.h"
#include <iostream>

std::string metainfo::getAnounce() {
    return anounce;
};
std::vector<std::string> metainfo::getAnounceList() {
    return anounce_list;
};
std::string metainfo::getComment() {
    return comment;
};
std::string metainfo::getCreator() {
    return created_by;
};
std::string metainfo::getCreationDate() {
    return creation_date;
};
std::string metainfo::getEncoding() {
    return encoding;
};
std::string metainfo::getName() {
    return name;
};

// basic info printing
void metainfo::printBasicInfo() {
    std::cout << this->getAnounce() << std::endl;
    std::cout << this->getComment() << std::endl;
    std::cout << this->getCreator() << std::endl;
    // std::cout << this->getCreationDate() << std::endl;
    std::cout << this->getEncoding() << std::endl;
    std::cout << this->getName() << std::endl;
};

metainfo::metainfo(BValue FullyDecodedObject) {
    for (const auto &[key, value] : FullyDecodedObject.asDict()) {
        if (key == "anounce") {
            anounce = value.asString();
        } else if (key == "comment") {
            comment = value.asString();
        } else if (key == "created by") {
            created_by = value.asString();
            // } else if (key == "creation date") {
            //     creation_date = value.asString();
        } else if (key == "encoding") {
            encoding = value.asString();
        } else if (key == "info") {
            fillInfo(FullyDecodedObject);
            // extractinfo
        }
    }
};

void metainfo::fillInfo(BValue FullyDecodedObject) {

    for (const auto &[key_1, value_1] : FullyDecodedObject.asDict()) {
        if (key_1 == "info") {
            for (const auto &[key_sub, value_sub] : value_1.asDict()) {
                if (key_sub == "name") {
                    name = value_sub.asString();
                } else if (key_sub == "piece length") {
                    piece_length = value_sub.asInt();
                } else if (key_sub == "pieces") {
                    pieces = value_sub.asString();
                }
            }
        }
    };
};
