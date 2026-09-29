#include "state_message_cmds.h"

std::string Byte_to_Str(uint32_t a)
{
    std::string status(1, static_cast<unsigned char>(a));
    return status;
}

std::string Short_to_Str(uint32_t a)
{
    std::string b1 = Byte_to_Str(((a >> 8) & 0xFF));
    std::string b2 = Byte_to_Str((a & 0xFF));
    std::string result = b1 + b2;
    return result;
}

std::string Word_to_Str(uint32_t a)
{
    std::string b1 = Byte_to_Str(((a >> 24) & 0xFF));
    std::string b2 = Byte_to_Str(((a >> 16) & 0xFF));
    std::string b3 = Byte_to_Str(((a >> 8) & 0xFF));
    std::string b4 = Byte_to_Str((a & 0xFF));
    std::string result = b1 + b2 + b3 + b4;
    return result;
}

uint32_t *Get_LVPS_data()
{
    static uint32_t state_msg_arr[64] = {0};
    std::string get_lvps_sample = "tail -n 3 "+LOG_DIR+"Client_LVPS.log > lvps_tmp.txt &";
    system(get_lvps_sample.c_str());
    usleep(100000);
    std::string filename = "lvps_tmp.txt";
    std::ifstream file;
    file.open(filename.c_str(), std::fstream::in);
    std::string lvps_sample;
    getline(file, lvps_sample);
    file.close();
    system("sudo rm lvps_tmp.txt");
    if(lvps_sample.size() != 641)
    {
        sleep(1);
        std::cout << "LVPS sample size: " << lvps_sample.size() << std::endl;
        std::cout << "Size of 1st HK Sample is wrong. Trying one more time ..." << std::endl;
        system(get_lvps_sample.c_str());
        usleep(100000);
        file.open(filename.c_str(), std::fstream::in);
        getline(file, lvps_sample);
        file.close();
        system("sudo rm lvps_tmp.txt");
        if(lvps_sample.size() != 641)
        {
            std::cout << "Size of 2nd HK Sample is wrong. Skipping this time." << std::endl;
            return state_msg_arr;
        }
    }
    for(int i=0; i<64; i++)
    {
        std::string tmp = lvps_sample.substr(i*10, 10);
        std::stringstream ss;
        ss << std::hex << tmp;
        ss >> state_msg_arr[i];
    }
    return state_msg_arr;
}
