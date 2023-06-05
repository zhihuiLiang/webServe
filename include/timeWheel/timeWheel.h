#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <list>

struct TimerNode {
    int id; // 定时器ID
    int expires; // 定时器到期时间
    std::function<void()> callback; // 定时器回调函数
};

class TimeWheel {
    using NodePtr = std::shared_ptr<TimerNode>;
public:
    TimeWheel(int slots_num, int interval);
    ~TimeWheel();
    void delTimer(int id);
    void tick();

    template<typename F, typename... Args>
    int addTimer(int expires, F&& f, Args&&... args);
private:
    uint32_t timer_id_;  //当前时间轮中定时器id
    uint32_t cur_slot_; // 当前时间轮指针所在槽位
    uint32_t slots_num_; // 时间轮槽数
    uint32_t interval_; // 每个槽位的时间间隔
    std::vector<std::list<NodePtr>> slots_; // 时间轮槽
    std::unordered_map<int, NodePtr> timers_; // 定时器ID到定时器节点的映射
};

template<typename F, typename... Args>
int TimeWheel::addTimer(int expires, F&& f, Args&&... args){
    auto node = std::make_shared<TimerNode>();
    node->id = ++timer_id_;
    node->expires = expires;
    node->callback = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    int slot = (cur_slot_ + (expires / interval_)) % slots_num_;
    slots_[slot].push_back(node);
    timers_[node->id] = node;
    return node->id;
}

