#include <string>
#include <vector>
class DeviceParser
{
public:
    std::vector<std::string> get_device_info()
    {
        return {get_name(),
                get_os_ver(),
                get_serial_number(),
                get_description()};
    }

protected:
    std::string get_name()
    {
        
    }

    std::string get_os_ver()
    {
    }
    std::string get_serial_number()
    {
    }
    std::string get_description()
    {
    }
};