#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

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
    void reserve(){
    int target_weekday[] = {2, 3, 6};
    constexpr int target_hour = 8;

    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);

    // 计算下一个目标时间点
    int days_until_target = -1;
    while (true) {
        int current_weekday = now_tm->tm_wday;
        int current_hour = now_tm->tm_hour;

        bool is_target_time = false;
        for (int i = 0; i < 3; ++i) {
            if (current_weekday == target_weekday[i] && current_hour == target_hour) {
                is_target_time = true;
                break;
            }
        }

        if (is_target_time) {
            // 执行任务
            task();

            // 等待一分钟
            std::this_thread::sleep_for(std::chrono::minutes(1));
        } else {
            // 计算下一个目标时间点距离当前时间的天数
            for(int i = 0; i < target_day.size(); ++i){
                if (day >= target_day[i])
            }
            days_until_target = target_weekday[0] - current_weekday;
            if (days_until_target <= 0) {
                days_until_target += 7;
            }

            // 等待到下一个目标时间点
            auto next_target_time = now + std::chrono::hours(24 * days_until_target)
                                    + std::chrono::hours(target_hour);
            now = std::chrono::system_clock::now();
            std::this_thread::sleep_until(next_target_time);
        }

        // 更新当前时间
        now = std::chrono::system_clock::now();
        now_time = std::chrono::system_clock::to_time_t(now);
        now_tm = std::localtime(&now_time);
    }
    }
}
}
;
