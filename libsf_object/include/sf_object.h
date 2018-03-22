#pragma once

#include <map>
#include <vector>
#include <thread>
#include <functional>
#include "sf_empty_class.h"
#include "sf_msg_queue.h"
#include "sf_eventloop.h"

#define SF_REG_SIGNAL(name,...)                                                                                         \
public:                                                                                                                 \
std::mutex __mu_##name##_signal_;                                                                                       \
std::vector<std::tuple<std::function<void(__VA_ARGS__)>, bool, int>>__##name##_signal_func_vec__;                        \
template<typename...__SF_OBJECT_ARGS__>                                                                                 \
void name(__SF_OBJECT_ARGS__&&... args) {                                                                               \
    std::lock_guard<std::mutex> lck(__mu_##name##_signal_);                                                             \
    for (auto &p :__##name##_signal_func_vec__)                                                                          \
    {                                                                                                                   \
        if (std::get<1>(p))                                                                                             \
        {                                                                                                               \
            std::get<0>(p)(std::forward<__SF_OBJECT_ARGS__>(args)...);                                                  \
        }                                                                                                               \
        else                                                                                                            \
        {                                                                                                               \
            auto bind_obj = std::bind(std::get<0>(p),                                                                   \
                                         std::forward<__SF_OBJECT_ARGS__>(args)...);                                    \
            std::thread([=]{                                                                                            \
                __p_msg_queue__->add_msg(this, bind_obj);                                                               \
            }).detach();                                                                                                \
        }                                                                                                               \
    }                                                                                                                   \
}                                                                                                                       \



#define SF_REG_AOP(name, ...)                                                                                           \
public:                                                                                                                 \
    std::mutex __mu_##name##_aop__;                                                                                     \
    std::vector<std::tuple<std::function<void(__VA_ARGS__)>,int>> __##name##_aop_before_func_vec__;                     \
    std::vector<std::tuple<std::function<void()>,int>> __##name##_aop_after_func_vec__;                                 \
    template<typename...__SF_OBJECT_ARGS__>                                                                             \
    decltype(auto) aop_##name(__SF_OBJECT_ARGS__&& ... args)                                                            \
    {                                                                                                                   \
        std::lock_guard<std::mutex> lck(__mu_##name##_aop__);                                                           \
        for(auto &p :  __##name##_aop_before_func_vec__)                                                                \
        {                                                                                                               \
            std::get<0>(p)(std::forward<__SF_OBJECT_ARGS__>(args)...);                                                  \
        }                                                                                                               \
        if constexpr (std::is_same<decltype(name(std::forward<__SF_OBJECT_ARGS__>(args)...)),void>::value)              \
        {                                                                                                               \
            name(std::forward<__SF_OBJECT_ARGS__>(args)...);                                                            \
            for(auto &p :  __##name##_aop_after_func_vec__)                                                             \
            {                                                                                                           \
                std::get<0>(p)();                                                                                       \
            }                                                                                                           \
        }                                                                                                               \
        else                                                                                                            \
        {                                                                                                               \
            decltype(auto) ret = name(std::forward<__SF_OBJECT_ARGS__>(args)...);                                       \
            for(auto &p :  __##name##_aop_after_func_vec__)                                                             \
            {                                                                                                           \
                std::get<0>(p)();                                                                                       \
            }                                                                                                           \
            return ret;                                                                                                 \
        }                                                                                                               \
    }                                                                                                                   \


#define sf_aop_before_bind(objptr, name, func)                                                                          \
(objptr)->__sf_aop_before_add_helper((objptr)->__mu_##name##_aop__,(objptr)->__##name##_aop_before_func_vec__,func)     \

#define sf_aop_after_bind(objptr, name, func)                                                                           \
(objptr)->__sf_aop_after_add_helper((objptr)->__mu_##name##_aop__,(objptr)->__##name##_aop_after_func_vec__,func)       \


#define sf_aop_before_unbind(objptr,name,bind_id)                                                                       \
(objptr)->__sf_aop_unbind_helper((objptr)->__mu_##name##_aop__,(objptr)->__##name##_aop_before_func_vec__,bind_id);     \


#define sf_aop_after_unbind(objptr,name,bind_id)                                                                        \
(objptr)->__sf_aop_unbind_helper((objptr)->__mu_##name##_aop__,(objptr)->__##name##_aop_after_func_vec__,bind_id);      \


#define sf_bind_signal(objptr,name,func,mul_thread)                                                                     \
(objptr)->__sf_bind_helper((objptr)->__mu_##name##_signal_,(objptr)->__##name##_signal_func_vec__,func,mul_thread)      \


#define sf_unbind_signal(objptr,name,bind_id)                                                                           \
(objptr)->__sf_signal_unbind_helper((objptr)->__mu_##name##_signal_,(objptr)->__##name##_signal_func_vec__,bind_id);    \


namespace skyfire
{
    template<typename _BaseClass = sf_empty_class>
    class sf_object : public _BaseClass
    {
    public:
        template<typename _VectorType, typename _FuncType>
        int __sf_bind_helper(std::mutex &mu,_VectorType &vec, _FuncType func, bool mul_thread)
        {
            std::lock_guard<std::mutex> lck(mu);
            int bind_id = rand();
            while(std::find_if(vec.begin(),vec.end(),[=](auto p){
                return std::get<2>(p) == bind_id;
            }) != vec.end())
            {
                bind_id = rand();
            }
            vec.push_back(std::make_tuple(func, mul_thread, bind_id));
            return bind_id;
        }

        template<typename _VectorType>
        void __sf_signal_unbind_helper(std::mutex &mu,_VectorType &vec, int bind_id)
        {
            std::lock_guard<std::mutex> lck(mu);
            vec.erase(std::remove_if(vec.begin(),vec.end(),[=](auto p){
                return std::get<2>(p) == bind_id;
            }), vec.end());
        }

        template<typename _VectorType, typename _FuncType>
        int __sf_aop_before_add_helper(std::mutex &mu,_VectorType &vec, _FuncType func)
        {
            std::lock_guard<std::mutex> lck(mu);
            int bind_id = rand();
            while(std::find_if(vec.begin(),vec.end(),[=](auto p){
                return std::get<1>(p) == bind_id;
            }) != vec.end())
            {
                bind_id = rand();
            }
            vec.insert(vec.begin() ,std::make_tuple(func, bind_id));
            return bind_id;
        }

        template<typename _VectorType, typename _FuncType>
        int __sf_aop_after_add_helper(std::mutex &mu,_VectorType &vec, _FuncType func)
        {
            std::lock_guard<std::mutex> lck(mu);
            int bind_id = rand();
            while(std::find_if(vec.begin(),vec.end(),[=](auto p){
                return std::get<1>(p) == bind_id;
            }) != vec.end())
            {
                bind_id = rand();
            }
            vec.push_back(std::make_tuple(func, bind_id));
            return bind_id;
        }

        template<typename _VectorType>
        void __sf_aop_unbind_helper(std::mutex &mu,_VectorType &vec, int bind_id)
        {
            std::lock_guard<std::mutex> lck(mu);
            vec.erase(std::remove_if(vec.begin(),vec.end(),[=](auto p){
                return std::get<1>(p) == bind_id;
            }), vec.end());
        }


        virtual ~sf_object()
        {
            __p_msg_queue__->remove_msg(this);
        }

    protected:
        sf_msg_queue* __p_msg_queue__ = sf_msg_queue::get_instance();
    };

}