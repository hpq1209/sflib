/*
 * sf_msg_queue 消息队列
 */

#pragma once

#include "sf_msg_queue.h"

namespace skyfire
{

    inline sf_msg_queue::sf_msg_queue() {}

    inline void sf_msg_queue::add_msg(void *id, std::function<void()> func) {
        std::lock_guard<std::mutex> lck(mu_func_data_op__);
        func_data__.push_back({id, func});
        wait_cond__.notify_all();
    }

    inline void sf_msg_queue::remove_msg(void *id) {
        std::lock_guard<std::mutex> lck(mu_func_data_op__);
        std::remove_if(func_data__.begin(),func_data__.end(),[=](const std::pair<void*,std::function<void()>> dt){
            return dt.first == id;
        });
    }

    inline void sf_msg_queue::clear() {
        std::lock_guard<std::mutex> lck(mu_func_data_op__);
        func_data__.clear();
    }

    inline std::function<void()> sf_msg_queue::take() {
        std::lock_guard<std::mutex> lck(mu_func_data_op__);
        if(func_data__.empty())
        {
            return std::function<void()>();
        }
        auto ret = func_data__.begin()->second;
        func_data__.pop_front();
        return ret;
    }

    inline bool sf_msg_queue::empty() {
        return func_data__.empty();
    }

    inline void sf_msg_queue::wait_msg() {
        std::unique_lock<std::mutex> lck(wait_mu__);
        wait_cond__.wait(lck);
    }

    inline void sf_msg_queue::add_empty_msg() {
        wait_cond__.notify_all();
    }
}