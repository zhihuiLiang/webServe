#pragma once
#include <chrono>
#include <codecvt>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "utility.h"
#include "webServer/webServer.h"

class AutoReserve {
    const std::vector<std::string> ground_id_ = { "1298272433186332673", "1297443858304540673", "1298272615009411073",
                                                  "1297443858304540673", "1298272791098875905", "1298273087183183874",
                                                  "1298273175146127362", "1298273265650819073", "1298273399927267330",
                                                  "1298273500317933570" };
    const std::vector<int> order_ = { 10, 9, 8, 7, 4, 3, 2, 1, 6, 5 };
    const std::string gym_id_ = "1297443858304540673";
    std::vector<int> target_weekday_ = { 3, 4, 7 };
    std::string start = " 09:30:00";
    std::string end = " 11:30:00";
    const int target_hour_ = 20;

    rapidjson::Document data_d_;
    std::stringstream header_;

    WebServer* srv_ptr_;

public:
    AutoReserve() {
        header_ << "Host: "
                << "reservation.ruichengyunqin.com\r\n";
        header_ << "User-Agent: "
                << "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 "
                   "Safari/537.36 NetType/WIFI MicroMessenger/7.0.20.1781(0x6700143B) WindowsWechat(0x63060012)\r\n";
        header_ << "Accept-Encoding: "
                << "gzip, deflate\r\n";
        // header_ << "Accept: " << "*/*\r\n";
        header_ << "Content-Type: "
                << "application/json\r\n";

        const char* data = R"xx({
            "customerId": "1427570050142879746",
            "customerName": "梁志辉",
            "customerTel": "18274705630",
            "userNum": 2,
            "customerEmail": "",
            "gymId": "1297443858304540673",
            "gymName": "润扬体育馆",
            "groundId": null,
            "groundName": null,
            "groundType": "0",
            "messagePushType": "0",
            "isIllegal": "0",
            "orderDate": null,
            "startTime": null,
            "endTime": null,
            "tmpOrderDate": null,
            "tmpStartTime": null,
            "tmpEndTime": null
        })xx";

        data_d_.Parse(data);
        checkJson(data_d_);
    }

    void initWebSrvPtr(WebServer* srv_ptr) {
        srv_ptr_ = srv_ptr;
    }
    void reserve() {
        if(target_weekday_.empty()) {
            std::cout << "target weekday is empty!" << std::endl;
            return;
        }

        const auto fmt_time = [](auto tm_point) {
            std::stringstream ss;
            std::time_t t1 = std::chrono::system_clock::to_time_t(tm_point);
            ss << std::put_time(std::localtime(&t1), "%Y-%m-%d");
            return ss.str();
        };

        while(true) {
            // 获取当前时间
            auto now = std::chrono::system_clock::now();
            std::time_t now_time = std::chrono::system_clock::to_time_t(now);
            std::tm* now_tm = std::localtime(&now_time);
            std::cout << "cur_time:" << std::ctime(&now_time);

            int cur_weekday = now_tm->tm_wday;
            int cur_hour = now_tm->tm_hour;

            // 计算下一个目标时间点距离当前时间的天数
            int next_day_idx = -1, nd = target_weekday_.size();
            for(int i = 0; i < nd; ++i) {
                if(target_weekday_[i] >= cur_weekday) {
                    next_day_idx = i;
                    break;
                }
            }
            int days_until_target = 0;
            if(next_day_idx == -1 || (next_day_idx == nd - 1 && cur_hour >= target_hour_)) {
                days_until_target += (7 - cur_weekday + target_weekday_[0]);
                next_day_idx = 0;
            } else {
                if(cur_hour >= target_hour_) {
                    next_day_idx += 1;
                }
                days_until_target = target_weekday_[next_day_idx] - cur_weekday;
            }

            // 等待到下一个目标时间点
            auto target = now + std::chrono::hours(24 * days_until_target) + std::chrono::hours(19) + std::chrono::minutes(59) -
                std::chrono::hours(cur_hour) - std::chrono::minutes(now_tm->tm_min) - std::chrono::seconds(now_tm->tm_sec);
            auto target_time = std::chrono::system_clock::to_time_t(target);
            auto rsv = target + std::chrono::hours(24 * 7);
            std::cout << "day_until:" << days_until_target << " target_time:" << std::ctime(&target_time);

            std::string order_date = fmt_time(target) + " 20:00:00";
            std::string start_time = fmt_time(rsv) + start;
            std::string end_time = fmt_time(rsv) + end;
            std::cout << order_date << " " << start_time << " " << end_time << std::endl;
            modifyDoc(data_d_, "orderDate", order_date.c_str());
            modifyDoc(data_d_, "startTime", start_time.c_str());
            modifyDoc(data_d_, "endTime", end_time.c_str());
            modifyDoc(data_d_, "tmpOrderDate", order_date.c_str());
            modifyDoc(data_d_, "tmpStartTime", start_time.c_str());
            modifyDoc(data_d_, "tmpEndTime", end_time.c_str());

            int n_ground = ground_id_.size();
            std::vector<std::string> contents(n_ground);
            for(int i = 0; i < n_ground; ++i) {
                std::string ground_name = std::to_string(order_[i]) + "号场";
                modifyDoc(data_d_, "groundName", ground_name.c_str());
                modifyDoc(data_d_, "groundId", ground_id_[order_[i]].c_str());
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                data_d_.Accept(writer);
                contents[i] = buffer.GetString();
            }
            std::this_thread::sleep_until(target);
            auto bev = srv_ptr_->connectSrv("8.129.5.136");
            now = std::chrono::system_clock::now();
            now_time = std::chrono::system_clock::to_time_t(now);
            now_tm = std::localtime(&now_time);
            std::string url = "http://reservation.ruichengyunqin.com/api/blade-app/qywx/saveOrder?userid=12132276";
            while(now_tm->tm_hour != target_hour_ && now_tm->tm_sec != 0)
                ;
            for(int i = 0; i < n_ground; ++i) {
                srv_ptr_->sendHttpReq(bev, "POST", url, header_.str(), contents[i]);
            }
            bufferevent_free(bev);
        }
    }
};
