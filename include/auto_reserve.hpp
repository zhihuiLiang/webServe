#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

class AutoReserve {
    const std::vector<std::string> gruond_id_ = { "1298272433186332673", "1297443858304540673", "1298272615009411073",
                                                  "1297443858304540673", "1298272791098875905", "1298273087183183874",
                                                  "1298273175146127362", "1298273265650819073", "1298273399927267330",
                                                  "1298273500317933570" };
    const std::vector<int> order_ = { 10, 9, 8, 7, 4, 3, 2, 1, 6, 5 };
    const std::string gym_id_ = "1297443858304540673";

    rapidjson::Document d_;
    const char *header_, *data_;

public:
    AutoReserve() {
        // header_ = R"({
        //     "Content-Type": "application/json",
        //     "User-Agent": "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36 NetType/WIFI MicroMessenger/7.0.20.1781(0x6700143B) WindowsWechat(0x63060012)",
        //     })";
        data_ = R"({
                    "customerId": "1427570050142879746",
                    "customerName": "梁志辉",
                    "customerTel": "18274705630",
                    "userNum": 2,
                    "customerEmail": "",
                    "gymId": "",
                    "gymName": "",
                    "groundId": "",
                    "groundName": "",
                    "groundType": "0",
                    "messagePushType": "0",
                    "isIllegal": "0",
                    "orderDate": "",
                    "startTime: "",
                    "endTime": "",
                    "tmpOrderDate": "",
                    "tmpStartTime": "",
                    "tmpEndTime": ""
        })";
}
}
;