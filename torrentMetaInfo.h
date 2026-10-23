#pragma once
#include "bencode_parser.h"
#include "commonheader.h"
#include <string>
#include <vector>

class metainfo {
  private:
    std::string anounce;
    std::vector<std::string> anounce_list;
    std::string comment;
    std::string created_by;
    std::string creation_date;
    std::string encoding;
    std::vector<uint8_t> pieces;

  public:
    // getters
    std::string getAnounce();
    std::vector<std::string> getAnounceList();
    std::string getComment();
    std::string getCreator();
    std::string getCreationDate();
    std::string getEncoding();

    // basic info printing
    void printBasicInfo();

    // filling object
    metainfo(BValue FullyDecodedObject);
};
