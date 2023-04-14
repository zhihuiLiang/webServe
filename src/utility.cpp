#include "utility.h"

void checkJson(const rapidjson::Document& doc) {
    if(doc.HasParseError()) {
        std::cout << "Error offset: " << doc.GetErrorOffset() << std::endl;
        std::cout << "Error description: " << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
    } else {
        std::cout << "JSON parsed successfully!" << std::endl;
    }
}

void modifyDoc(rapidjson::Document& doc, const char* key, const char* val) {
    if(!doc.IsObject()) {
        std::cerr << "doc is not a json object" << std::endl;
        return;
    }
    if(doc.HasMember(key)) {
        doc[key].SetString(val, strlen(val));
    } else {
        std::cerr << "doc has no key" << key << std::endl;
    }
}

void modifyDoc(rapidjson::Document& doc, const char* key, int val){
    if(!doc.IsObject()) {
        std::cerr << "doc is not a json object" << std::endl;
        return;
    }
    if(doc.HasMember(key)) {
        doc[key].SetInt(val);
    } else {
        std::cerr << "doc has no key" << key << std::endl;
    }
}
