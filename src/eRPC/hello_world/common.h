#include <stdio.h>
#include "rpc.h"

static const std::string kServerHostname = "129.215.165.5"; // "130.127.133.179";
static const std::string kClientHostname = "129.215.165.6"; //"130.127.133.145";

static constexpr uint16_t kUDPPort = 31850;
static constexpr uint8_t kReqType = 2;
static constexpr size_t kMsgSize = 16;
