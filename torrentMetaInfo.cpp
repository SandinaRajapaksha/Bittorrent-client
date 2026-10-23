#include "torrentMetaInfo.h"
#include "commonheader.h"

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

// basic info printing
void metainfo::printBasicInfo() {
    std::cout << this->getAnounce() << std::endl;
    std::cout << this->getComment() << std::endl;
    std::cout << this->getCreator() << std::endl;
    // std::cout << this->getCreationDate() << std::endl;
    std::cout << this->getEncoding() << std::endl;
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
        }
    }
};
