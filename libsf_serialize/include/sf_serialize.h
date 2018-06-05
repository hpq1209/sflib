/*
 * 提供序列化支持（二进制）
 */

#pragma once

#include <type_traits>
#include <vector>
#include <exception>
#include <string>
#include <cstring>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include "sf_type.h"

namespace skyfire
{
    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, byte_array>::type sf_serialize(const _Pod_Type &value);

    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, size_t>::type
    sf_deserialize(const byte_array &data, _Pod_Type &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize(const std::vector <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::vector <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize(const std::list <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::list <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize(const std::deque <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::deque <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize(const std::set <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::set <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize(const std::unordered_set <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::unordered_set <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize(const std::multiset <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::multiset <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize(const std::basic_string <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::basic_string <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize(const std::unordered_multiset <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::unordered_multiset <_Type> &obj, size_t begin_pos);

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize(const std::unordered_multimap <_TypeKey, _TypeValue> &obj);

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize(const byte_array &data, std::unordered_multimap <_TypeKey, _TypeValue> &obj, size_t begin_pos);

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize(const std::unordered_map <_TypeKey, _TypeValue> &obj);

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize(const byte_array &data, std::unordered_map <_TypeKey, _TypeValue> &obj, size_t begin_pos);

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize(const std::multimap <_TypeKey, _TypeValue> &obj);

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize(const byte_array &data, std::multimap <_TypeKey, _TypeValue> &obj, size_t begin_pos);

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize(const std::map <_TypeKey, _TypeValue> &obj);

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize(const byte_array &data, std::map <_TypeKey, _TypeValue> &obj, size_t begin_pos);

    template<typename _First_Type, typename... _Types>
    byte_array sf_serialize(const _First_Type &first, const _Types &... value);

    template<int N, typename... _Types>
    typename std::enable_if<N == sizeof...(_Types), byte_array>::type
    sf_serialize_tuple(const std::tuple<_Types...> &obj);

    template<int N, typename... _Types>
    typename std::enable_if<N != sizeof...(_Types), byte_array>::type
    sf_serialize_tuple(const std::tuple<_Types...> &obj);

    template<typename... _Types>
    byte_array sf_serialize(const std::tuple<_Types...> &obj);

    template<int N, typename _Tuple_Type, typename... _Types>
    typename std::enable_if<N == std::tuple_size<_Tuple_Type>::value, size_t>::type
    sf_deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg);

    template<int N, typename _Tuple_Type, typename... _Types>
    typename std::enable_if<N != std::tuple_size<_Tuple_Type>::value, size_t>::type
    sf_deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg);

    template<typename _Tuple_Type>
    size_t sf_deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj);

    template<typename... _Types>
    size_t sf_deserialize(const byte_array &data, std::tuple<_Types...> &obj, size_t begin_pos);

    inline byte_array operator+(byte_array b1, const byte_array &b2)
    {
        b1.insert(b1.end(), b2.begin(), b2.end());
        return b1;
    }

    inline byte_array &operator+=(byte_array &b1, const byte_array &b2)
    {
        b1.insert(b1.end(), b2.begin(), b2.end());
        return b1;
    }

    class sf_serialize_size_mismatch_exception : public std::exception
    {
    public:
        sf_serialize_size_mismatch_exception(const std::string &exception_message) : _message(exception_message)
        {
        }

        const char *what() const noexcept override
        {
            return _message.c_str();
        }

    private:
        std::string _message;
    };

    inline byte_array sf_serialize()
    {
        return byte_array();
    }

    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, byte_array>::type sf_serialize(const _Pod_Type &value)
    {
        byte_array ret(sizeof(_Pod_Type));
        auto p = reinterpret_cast<const char *>(&value);
        memcpy(ret.data(), &value, sizeof(_Pod_Type));
        return ret;
    }

    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, size_t>::type
    sf_deserialize(const byte_array &data, _Pod_Type &obj, size_t begin_pos)
    {
        if (sizeof(_Pod_Type) > data.size() - begin_pos)
        {
            throw sf_serialize_size_mismatch_exception("Data size is to small");
        }
        memcpy(&obj, data.data() + begin_pos, sizeof(_Pod_Type));
        return begin_pos + sizeof(_Pod_Type);
    }

    template<typename _Type>
    byte_array sf_serialize(const std::vector <_Type> &value)
    {
        byte_array ret;
        size_t len = value.size();
        auto tmp_ret = sf_serialize(len);
        ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
        for (auto const &p : value)
        {
            tmp_ret = sf_serialize(p);
            ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
        }
        return ret;
    }

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::vector <_Type> &obj,
                          size_t begin_pos)
    {
        obj.clear();
        size_t len;
        auto offset = sf_deserialize(data, len, begin_pos);
        obj.resize(len);
        for (auto i = 0; i < len; ++i)
        {
            offset = sf_deserialize(data, obj[i], offset);
        }
        return offset;
    }

    template<typename _Type>
    byte_array sf_serialize(const std::list <_Type> &value)
    {
        std::vector <_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::list <_Type> &obj,
                          size_t begin_pos)
    {
        std::vector <_Type> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::list<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize(const std::deque <_Type> &value)
    {
        std::vector <_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::deque <_Type> &obj,
                          size_t begin_pos)
    {
        std::vector <_Type> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::deque<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize(const std::set <_Type> &value)
    {
        std::vector <_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::set <_Type> &obj,
                          size_t begin_pos)
    {
        std::vector <_Type> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::set<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize(const std::unordered_set <_Type> &value)
    {
        std::vector <_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::unordered_set <_Type> &obj,
                          size_t begin_pos)
    {
        std::vector <_Type> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::unordered_set<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize(const std::multiset <_Type> &value)
    {
        std::vector <_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::multiset <_Type> &obj,
                          size_t begin_pos)
    {
        std::vector <_Type> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::multiset<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize(const std::basic_string <_Type> &value)
    {
        std::vector<char> tmp_obj(value.begin(), value.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::basic_string <_Type> &obj,
                          size_t begin_pos)
    {
        std::vector<char> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::basic_string<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize(const std::unordered_multiset <_Type> &value)
    {
        std::vector <_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize(const byte_array &data, std::unordered_multiset <_Type> &obj,
                          size_t begin_pos)
    {
        std::vector <_Type> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::unordered_multiset<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize(const std::unordered_multimap <_TypeKey, _TypeValue> &obj)
    {
        byte_array ret;
        size_t len = obj.size();
        auto tmp_ret = sf_serialize(len);
        ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
        for (auto const &p : obj)
        {
            tmp_ret = sf_serialize(p.first);
            ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
            tmp_ret = sf_serialize(p.second);
            ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
        }
        return ret;
    }

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize(const byte_array &data, std::unordered_multimap <_TypeKey, _TypeValue> &obj,
                   size_t begin_pos)
    {
        obj.clear();
        size_t len;
        auto offset = sf_deserialize(data, len, begin_pos);
        _TypeKey key;
        _TypeValue value;
        for (auto i = 0; i < len; ++i)
        {
            offset = sf_deserialize(data, key, offset);
            offset = sf_deserialize(data, value, offset);
            obj.insert({key, value});
        }
        return offset;
    }

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize(const std::unordered_map <_TypeKey, _TypeValue> &obj)
    {
        std::unordered_multimap <_TypeKey, _TypeValue> tmp_obj(obj.begin(), obj.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize(const byte_array &data, std::unordered_map <_TypeKey, _TypeValue> &obj,
                   size_t begin_pos)
    {
        std::unordered_multimap <_TypeKey, _TypeValue> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::unordered_map<_TypeKey, _TypeValue>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize(const std::multimap <_TypeKey, _TypeValue> &obj)
    {
        std::unordered_multimap <_TypeKey, _TypeValue> tmp_obj(obj.begin(), obj.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize(const byte_array &data, std::multimap <_TypeKey, _TypeValue> &obj,
                   size_t begin_pos)
    {
        std::unordered_multimap <_TypeKey, _TypeValue> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::multimap<_TypeKey, _TypeValue>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize(const std::map <_TypeKey, _TypeValue> &obj)
    {
        std::unordered_multimap <_TypeKey, _TypeValue> tmp_obj(obj.begin(), obj.end());
        return sf_serialize(tmp_obj);
    }

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize(const byte_array &data, std::map <_TypeKey, _TypeValue> &obj,
                   size_t begin_pos)
    {
        std::unordered_multimap <_TypeKey, _TypeValue> tmp_obj;
        auto ret = sf_deserialize(data, tmp_obj, begin_pos);
        obj = std::map<_TypeKey, _TypeValue>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _First_Type, typename... _Types>
    byte_array sf_serialize(const _First_Type &first, const _Types &... value)
    {
        return sf_serialize(first) + sf_serialize(value...);
    }

    template<int N, typename... _Types>
    typename std::enable_if<N == sizeof...(_Types), byte_array>::type
    sf_serialize_tuple(const std::tuple<_Types...> &obj)
    {
        return byte_array();
    }

    template<int N, typename... _Types>
    typename std::enable_if<N != sizeof...(_Types), byte_array>::type
    sf_serialize_tuple(const std::tuple<_Types...> &obj)
    {
        return sf_serialize(std::get<N>(obj)) + sf_serialize_tuple<N + 1, _Types...>(obj);
    }

    template<typename... _Types>
    byte_array sf_serialize(const std::tuple<_Types...> &obj)
    {
        return sf_serialize_tuple<0, _Types...>(obj);
    }

    template<int N, typename _Tuple_Type, typename... _Types>
    typename std::enable_if<N == std::tuple_size<_Tuple_Type>::value, size_t>::type
    sf_deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg)
    {
        obj = std::tuple<_Types...>(arg...);
        return pos;
    }

    template<int N, typename _Tuple_Type, typename... _Types>
    typename std::enable_if<N != std::tuple_size<_Tuple_Type>::value, size_t>::type
    sf_deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg)
    {
        using next_type = typename std::remove_reference<decltype(std::get<N>(std::declval<_Tuple_Type>()))>::type;
        next_type _t;
        pos = sf_deserialize(data, _t, pos);
        return sf_deserialize_tuple<N + 1, _Tuple_Type, _Types..., next_type>(pos, data, obj, arg..., _t);
    }

    template<typename _Tuple_Type>
    size_t sf_deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj)
    {
        using next_type = typename std::remove_reference<decltype(std::get<0>(std::declval<_Tuple_Type>()))>::type;
        next_type _t;
        pos = sf_deserialize(data, _t, pos);
        return sf_deserialize_tuple<1, _Tuple_Type, next_type>(pos, data, obj, _t);
    }

    template<typename... _Types>
    size_t sf_deserialize(const byte_array &data, std::tuple<_Types...> &obj, size_t begin_pos)
    {
        if constexpr (sizeof...(_Types) == 0)
        {
            return begin_pos;
        }
        else
        {
            return sf_deserialize_tuple(begin_pos, data, obj);
        }
    }



}


