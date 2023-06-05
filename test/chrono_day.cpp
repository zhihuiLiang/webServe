#include <iostream>
#include <chrono>
#include <ctime>

int main() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch();
    long duration = value.count();
    std::cout << "当前日期为：" << now_tm->tm_year + 1900 << "-" << now_tm->tm_mon + 1 << "-" << now_tm->tm_mday << std::endl;
    std::cout << "当前时间为：" << now_tm->tm_hour << ":" << now_tm->tm_min << ":" << now_tm->tm_sec << "." << duration % 1000 << std::endl;
    return 0;
}