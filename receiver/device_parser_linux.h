#pragma once
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <fstream>

#include <string>
#include <vector>


#include "messages.pb.h"

class DeviceParser
{
public:
    static coolProtocol::DeviceInfo get_device_info()
    {
        coolProtocol::DeviceInfo d;
        
        d.set_allocated_name(new std::string(get_name()));
        d.set_allocated_os_ver(new std::string(get_os_ver()));
        d.set_allocated_part_number(new std::string(get_serial_number()));
        assign_description(d);

        return d;
    }

protected:
    static std::string get_name()
    {
        //const size_t HOST_NAME_MAX = 100;
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, HOST_NAME_MAX);
        return std::string(hostname);
    }

    static std::string get_os_ver()
    {
        struct utsname buffer;
        uname(&buffer);
        return std::string(buffer.sysname) + " " + std::string(buffer.release);
    }

    static std::string get_serial_number()
    {
        std::ifstream ifs("/sys/class/dmi/id/product_serial");
        std::string serial;

        //of course I could to just not set it in message, but that one I like more
        if (!ifs)
        {
            serial = "NO SERIAL";
        }

        std::getline(ifs, serial);
        if (ifs.fail())
        {
            serial = "NO SERIAL";
        }

        return serial;
    }

    static void assign_description(coolProtocol::DeviceInfo& d)
    {
        struct sysinfo si;
        sysinfo(&si);
        // printf("Uptime : %ld\n", si.uptime);
        // printf("Free RAM : %ld\n", si.freeram);
        // printf("Total RAM : %ld\n", si.totalram);
        // printf("Process count : %d\n", si.procs);

        d.add_desc(std::to_string(si.uptime));
        d.add_desc(std::to_string(si.freeram));
        d.add_desc(std::to_string(si.totalram));
        d.add_desc(std::to_string(si.procs));
    }
};