#ifndef _STATE_MSG_H
#define _STATE_MSG_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <functional>
#include "constants.h"

using namespace std;

std::string Byte_to_Str(uint32_t a);
std::string Short_to_Str(uint32_t a);
std::string Word_to_Str(uint32_t a);
uint32_t *Get_LVPS_data();

#endif