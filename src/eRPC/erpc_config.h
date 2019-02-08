#include <stdio.h>
#include "rpc.h"

static const std::string kServerHostname = "192.168.122.14";
static const std::string kClientHostname = "192.168.122.103";

static constexpr uint16_t worker_port = 31850;
static constexpr uint16_t client_port = 31860;

static constexpr uint8_t kReqType = 2;
static constexpr size_t kMsgSize = 16;
