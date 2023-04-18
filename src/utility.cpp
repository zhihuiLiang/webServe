#include "utility.h"
#include "log/log.h"

bool checkJson(const rapidjson::Document& doc) {
    if(doc.HasParseError()) {
        std::cout << "Error offset: " << doc.GetErrorOffset() << std::endl;
        std::cout << "Error description: " << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
        return true;
    } else {
        std::cout << "JSON parsed successfully!" << std::endl;
        return false;
    }
}

bool modifyDoc(rapidjson::Document& doc, const char* key, const char* val) {
    if(!doc.IsObject()) {
        std::cerr << "doc is not a json object" << std::endl;
        return false;
    }
    if(doc.HasMember(key)) {
        doc[key].SetString(val, strlen(val));
        
    } else {
        rapidjson::Value k(key, doc.GetAllocator()), v(val, doc.GetAllocator());
        doc.AddMember(k, v, doc.GetAllocator());
    }   
    return true;
}

bool modifyDoc(rapidjson::Document& doc, const char* key, int val){
    if(!doc.IsObject()) {
        std::cerr << "doc is not a json object" << std::endl;
        return false;
    }
    if(doc.HasMember(key)) {
        doc[key].SetInt(val);
       
    } else {
        rapidjson::Value k(key, doc.GetAllocator()), v(val);
        doc.AddMember(k, v, doc.GetAllocator());
    }
     return true;
}
