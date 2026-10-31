#pragma once
#include "bencode_parser.h"
#include "commonheader.h"
#include <cstdint>
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

    // info layer
    std::string name;
    uint64_t piece_length;
    std::string pieces;

  public:
    // getters
    std::string getAnounce();
    std::vector<std::string> getAnounceList();
    std::string getComment();
    std::string getCreator();
    std::string getCreationDate();
    std::string getEncoding();
    std::string getName();

    // basic info printing
    void printBasicInfo();

    // filling object
    metainfo(BValue FullyDecodedObject);

    // filling info layer
    void fillInfo(BValue FullyDecodedObject);
};
