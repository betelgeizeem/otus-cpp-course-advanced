#include "version_counter.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

using ip_type = std::vector<std::string>;

ip_type split(const std::string &str, char d)
{
    ip_type r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

std::vector<ip_type> get_ip_pool()
{
    std::vector<ip_type> ip_pool;

    for(std::string line; std::getline(std::cin, line);)
    {
        auto v = split(line, '\t');
        ip_pool.push_back(split(v.at(0), '.'));
    }

    return ip_pool;
}


size_t const ip_size = 4;
using ip_type_numeric = std::array<size_t, ip_size>;
ip_type_numeric get_numeric(const ip_type& ip)
{
    if (ip.size() != ip_size)
    {
        auto str = std::string();
        for (const auto& s : ip)
        {
            str.append(s); 
            str.append(" ");
        }
        throw std::invalid_argument("bad ip get_numeric, ip: " + std::to_string(ip.size()));
    }
    ip_type_numeric result;
    for (size_t i = 0; i < ip_size; ++i)
        result[i] = std::stoi(ip[i]);
    
    return result;
}


void write_ip_pool(const std::vector<ip_type>& ip_pool)
{
    for(auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
    {
        for(auto ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
        {
            if (ip_part != ip->cbegin())
                std::cout << ".";
            std::cout << *ip_part;
        }
        std::cout << std::endl;
    }
}

struct numeric_filterer
{
    template<size_t N>
    bool operator()(const ip_type& ip, std::array<size_t, N> filter)
    {
        if (N > ip_size)
        {
            std::cout << ip[ip_size];
            throw std::invalid_argument("bad ip numeric_filterer");
        }
        auto ip_num = get_numeric(ip);
        
        for (size_t i = 0; i < N; ++i)
            if (ip_num[i] != filter[i])
                return false;
        return true;
    }

    bool operator()(const ip_type& ip, size_t n)
    {
        auto ip_num = get_numeric(ip);
        
        return std::any_of(ip_num.begin(), ip_num.end(), [n](auto i){ return i == n; });
    }
} filterer;

int main(int, char **)
{
    try
    {
        auto ip_pool = get_ip_pool();
       
        //std::cout << "-- sorted" << std::endl;
        std::sort(ip_pool.begin(), ip_pool.end(), [](const ip_type& ip1, const ip_type& ip2)
        {
            auto ip1_num = get_numeric(ip1);
            auto ip2_num = get_numeric(ip2);

            for (size_t i = 0; i < ip_size; ++i)
                if (ip1_num[i] != ip2_num[i])
                    return ip1_num[i] > ip2_num[i];
            return false;
        }); 

        write_ip_pool(ip_pool);

        //std::cout << "-- filter 1st by 1" << std::endl;
        {
            decltype(ip_pool) filtered_pool;
            std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(filtered_pool),
                [](const auto& ip) { return filterer(ip, std::array<size_t, 1>{1}); });
            write_ip_pool(filtered_pool);
        }

        //std::cout << "-- filter 1st, 2nd by 46, 70" << std::endl;
        {
            decltype(ip_pool) filtered_pool;
            std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(filtered_pool),
                [](const auto& ip) { return filterer(ip, std::array<size_t, 2>{46, 70}); });
            write_ip_pool(filtered_pool);
        }

        //std::cout << "-- filter any by 46" << std::endl;
        {
            decltype(ip_pool) filtered_pool;
            std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(filtered_pool),
                [](const auto& ip) { return filterer(ip, 46); });
            write_ip_pool(filtered_pool);
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
