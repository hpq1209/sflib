#pragma once

#include "sf_http_utils.h"
#include "sf_tcputils.hpp"
#include "sf_type.hpp"
#include "sf_http_request_line.h"
#include "sf_http_multipart.hpp"

namespace skyfire
{

    inline unsigned char sf_to_hex(unsigned char x)
    {
        return static_cast<unsigned char>(x > 9 ? x + 55 : x + 48);
    }

    inline unsigned char sf_from_hex(unsigned char x)
    {
        unsigned char y = 0;
        if (x >= 'A' && x <= 'Z') y = static_cast<unsigned char>(x - 'A' + 10);
        else if (x >= 'a' && x <= 'z') y = static_cast<unsigned char>(x - 'a' + 10);
        else if (x >= '0' && x <= '9') y = x - '0';
        return y;
    }

    inline std::string sf_url_encode(const std::string &str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (isalnum((unsigned char)str[i]) ||
                (str[i] == '-') ||
                (str[i] == '_') ||
                (str[i] == '.') ||
                (str[i] == '~'))
                strTemp += str[i];
            else if (str[i] == ' ')
                strTemp += "+";
            else
            {
                strTemp += '%';
                strTemp += sf_to_hex((unsigned char)str[i] >> 4);
                strTemp += sf_to_hex((unsigned char)str[i] % 16);
            }
        }
        return strTemp;
    }

    inline std::string sf_url_decode(const std::string &str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (str[i] == '+') strTemp += ' ';
            else if (str[i] == '%')
            {
                unsigned char high = sf_from_hex((unsigned char)str[++i]);
                unsigned char low = sf_from_hex((unsigned char)str[++i]);
                strTemp += high*16 + low;
            }
            else strTemp += str[i];
        }
        return strTemp;
    }


    inline void sf_parse_param(std::map<std::string, std::string> param,std::string &param_str) {
        param.clear();
        unsigned long url_pos;
        while((url_pos = param_str.find('&')) != std::string::npos)
        {
            auto tmp_param = std::string(param_str.begin(),param_str.begin()+url_pos);
            param_str = std::string(param_str.begin()+url_pos+1,param_str.end());
            if(tmp_param.empty())
                continue;
            if((url_pos = tmp_param.find('=')) == std::string::npos)
                continue;
            auto key = sf_url_decode(std::string(tmp_param.begin(), tmp_param.begin() + url_pos));
            auto value = sf_url_decode(std::string(tmp_param.begin() + url_pos + 1, tmp_param.end()));
            param[key] = value;
        }
        if(param_str.empty())
            return;
        if((url_pos = param_str.find('=')) == std::string::npos)
            return;
        auto key = sf_url_decode(std::string(param_str.begin(), param_str.begin() + url_pos));
        auto value = sf_url_decode(std::string(param_str.begin() + url_pos + 1, param_str.end()));
        param[key] = value;
    }


    inline void sf_parse_url(const std::string &raw_url, std::string &url, std::map<std::string,std::string>& param,
                             std::string frame)
    {
        auto frame_pos = raw_url.find('#');
        std::string raw_url_without_frame;
        if(frame_pos == std::string::npos) {
            raw_url_without_frame = raw_url;
            frame = "";
        }
        else{
            raw_url_without_frame = std::string(raw_url.begin(),raw_url.begin()+frame_pos);
            frame = std::string(raw_url.begin()+frame_pos+1,raw_url.end());
        }
        auto url_pos = raw_url_without_frame.find('?');
        if(url_pos == std::string::npos){
            url = raw_url_without_frame;
            return;
        }
        url = std::string(raw_url_without_frame.begin(),raw_url_without_frame.begin()+url_pos);
        auto param_str = std::string(raw_url_without_frame.begin()+url_pos+1,raw_url_without_frame.end());
        sf_parse_param(param, param_str);
    }


    inline std::string sf_make_http_time_str()
    {
        return sf_make_http_time_str(std::chrono::system_clock::now());
    }

    inline std::string sf_to_header_key_format(std::string key) {
        bool flag = false;
        for (auto &k:key) {
            if (0 != isalnum(k)) {
                if (!flag)
                    k = static_cast<char>(toupper(k));
                flag = true;
            } else {
                flag = false;
            }
        }
        return key;
    }

    inline std::string sf_make_http_time_str(const std::chrono::system_clock::time_point &tp)
    {
        auto raw_time = std::chrono::system_clock::to_time_t(tp);
        std::tm *time_info = std::localtime(&raw_time);
        std::string ret(128,'\0');
        strftime(ret.data(),128,"%a, %d %b %Y %T GMT",time_info);
        ret.resize(strlen(ret.c_str()));
        return ret;
    }

    inline byte_array read_file(const std::string &filename, size_t max_size)
    {
        byte_array data;
        std::ifstream fi(filename, std::ios::in | std::ios::binary);
        if(!fi){
            return data;
        }
        fi.seekg(0,std::ios::end);
        auto file_size = fi.tellg();
        if(file_size>max_size)
        {
            file_size = max_size;
        }
        fi.seekg(0,std::ios::beg);
        data.resize(file_size);
        fi.read(data.data(),file_size);
        fi.close();
        return data;
    }

}