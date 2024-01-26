#pragma once
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <fstream>

#include <string>
#include <vector>

struct DeviceInfo
{
    std::string device_name;
    std::string os_ver;
    std::string part_number;
    std::vector<std::string> device_description;
};

class DeviceParser
{
public:
    static DeviceInfo get_device_info()
    {
        return {get_name(),
                get_os_ver(),
                get_serial_number(),
                get_description()};
    }

protected:
    std::string get_name()
    {
        const size_t HOST_NAME_MAX = 100;
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, HOST_NAME_MAX);
        return std::string(hostname);
    }

    std::string get_os_ver()
    {
        struct utsname buffer;
        uname(&buffer);
        return std::string(buffer.sysname) + " " + std::string(buffer.release);
    }

std::string get_serial_number()
{
    std::ifstream ifs("/sys/class/dmi/id/product_serial");
    std::string serial;

    if (!ifs) {
        serial = "NO SERIAL";
    }

    
    std::getline(ifs, serial);
    if (ifs.fail()) {
        serial = "NO SERIAL";
    }

    return serial;
}

    std::vector<std::string> get_description()
    {
        struct sysinfo si;
        sysinfo(&si);
        // printf("Uptime : %ld\n", si.uptime);
        // printf("Free RAM : %ld\n", si.freeram);
        // printf("Total RAM : %ld\n", si.totalram);
        // printf("Process count : %d\n", si.procs);
        return {std::to_string(si.uptime),
                std::to_string(si.freeram),
                std::to_string(si.totalram),
                std::to_string(si.procs)};
    }
};