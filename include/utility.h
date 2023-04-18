#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <iostream>

extern bool checkJson(const rapidjson::Document& doc);
extern bool modifyDoc(rapidjson::Document& doc, const char* key, const char* val);
extern bool modifyDoc(rapidjson::Document& doc, const char* key, int val);