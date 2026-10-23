#include "bencode_parser.h"
#include "commonheader.h"
#include "torrentMetaInfo.h"
int main() {
    decoder harmoniaDecoder;
    BValue decodedTorrentInfo = harmoniaDecoder.decode();
    metainfo MetaInfoOfTorrent(decodedTorrentInfo);
    MetaInfoOfTorrent.printBasicInfo();
}
