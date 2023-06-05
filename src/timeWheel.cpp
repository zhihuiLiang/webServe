#include "timeWheel/timeWheel.h"

#include <thread>

using namespace std::literals;

TimeWheel::TimeWheel(int slots_num, int interval) : cur_slot_(0), slots_num_(slots_num), interval_(interval) {
    slots_.resize(slots_num_);
}

void TimeWheel::delTimer(int id) {
    auto it = timers_.find(id);
    if (it != timers_.end()) {
        auto node = it->second;
        int slot = (node->expires / interval_) % slots_num_;
        slots_[slot].remove(node);
        timers_.erase(it);
    }
}


void TimeWheel::tick() {
    while(1){
        std::list<NodePtr>& slot = slots_[cur_slot_];
        for (auto it = slot.begin(); it != slot.end(); ) {
            NodePtr node = *it;
            if (node->expires <= 0) {
                node->callback();
                timers_.erase(node->id);
                it = slot.erase(it);
            } else {
                --node->expires;
                ++it;
            }
        }
        std::this_thread::sleep_for(600ms);
        cur_slot_ = (cur_slot_ + 1) % slots_num_;
    }
    
}
