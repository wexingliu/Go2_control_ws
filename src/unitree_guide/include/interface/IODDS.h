#ifndef UNITREE_GUIDE_IODDS_H
#define UNITREE_GUIDE_IODDS_H

#include "interface/IOInterface.h"
#include <unitree/robot/channel/channel_publisher.hpp>
#include <unitree/robot/channel/channel_subscriber.hpp>
#include <unitree/idl/go2/LowCmd_.hpp>
#include <unitree/idl/go2/LowState_.hpp>
#include <chrono>
#include <mutex>
#include <string>

class IODDS : public IOInterface {
public:
    IODDS(const std::string &network_interface, int domain);
    void waitForState();
    void sendRecv(const LowlevelCmd *cmd, LowlevelState *state) override;
private:
    void onState(const void *message);
    std::mutex mutex_;
    unitree_go::msg::dds_::LowState_ state_{};
    unitree_go::msg::dds_::LowCmd_ cmd_{};
    std::chrono::steady_clock::time_point last_state_{};
    unitree::robot::ChannelPublisherPtr<unitree_go::msg::dds_::LowCmd_> publisher_;
    unitree::robot::ChannelSubscriberPtr<unitree_go::msg::dds_::LowState_> subscriber_;
};
#endif
