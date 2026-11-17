#include "interface/IODDS.h"
#include "interface/KeyBoard.h"
#include "interface/crc32.h"
#include <cmath>
#include <stdexcept>
#include <thread>
#include <iostream>

IODDS::IODDS(const std::string &network_interface, int domain) {
    cmdPanel = new KeyBoard();
    unitree::robot::ChannelFactory::Instance()->Init(domain, network_interface);
    publisher_.reset(new unitree::robot::ChannelPublisher<unitree_go::msg::dds_::LowCmd_>("rt/lowcmd"));
    publisher_->InitChannel();
    subscriber_.reset(new unitree::robot::ChannelSubscriber<unitree_go::msg::dds_::LowState_>("rt/lowstate"));
    subscriber_->InitChannel([this](const void *msg) { onState(msg); }, 1);
    cmd_.head()[0] = 0xfe;
    cmd_.head()[1] = 0xef;
    cmd_.level_flag() = 0xff;
    cmd_.gpio() = 0;
    for (auto &motor : cmd_.motor_cmd()) motor.mode() = 0x01;
}

void IODDS::onState(const void *message) {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = *static_cast<const unitree_go::msg::dds_::LowState_ *>(message);
    last_state_ = std::chrono::steady_clock::now();
}

void IODDS::waitForState() {
    std::cout << "Waiting for Go2 rt/lowstate (10 s timeout)..." << std::endl;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (std::chrono::steady_clock::now() < deadline) {
        { std::lock_guard<std::mutex> lock(mutex_);
          if (last_state_ != std::chrono::steady_clock::time_point{}) return; }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    throw std::runtime_error("No Go2 lowstate received; check network interface and DDS domain");
}

void IODDS::sendRecv(const LowlevelCmd *cmd, LowlevelState *state) {
    unitree_go::msg::dds_::LowState_ snapshot;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (last_state_ == std::chrono::steady_clock::time_point{} ||
            std::chrono::steady_clock::now() - last_state_ > std::chrono::milliseconds(100))
            throw std::runtime_error("Go2 lowstate timed out; stopped publishing commands");
        snapshot = state_;
    }
    for (int i = 0; i < 12; ++i) {
        const auto &in = snapshot.motor_state()[i];
        if (!std::isfinite(in.q()) || !std::isfinite(in.dq()) || !std::isfinite(in.tau_est()))
            throw std::runtime_error("Non-finite motor feedback; stopped publishing commands");
        auto &out = state->motorState[i];
        out.mode = in.mode(); out.q = in.q(); out.dq = in.dq(); out.ddq = in.ddq(); out.tauEst = in.tau_est();
        const auto &source = cmd->motorCmd[i];
        if (!std::isfinite(source.q) || !std::isfinite(source.dq) || !std::isfinite(source.tau) ||
            !std::isfinite(source.Kp) || !std::isfinite(source.Kd))
            throw std::runtime_error("Non-finite motor command; stopped publishing commands");
        auto &motor = cmd_.motor_cmd()[i];
        motor.mode() = 0x01; motor.q() = source.q; motor.dq() = source.dq;
        motor.kp() = source.Kp; motor.kd() = source.Kd; motor.tau() = source.tau;
    }
    double quaternion_norm = 0;
    for (int i = 0; i < 4; ++i) {
        const auto q = snapshot.imu_state().quaternion()[i];
        if (!std::isfinite(q)) throw std::runtime_error("Invalid IMU quaternion");
        quaternion_norm += q * q;
        state->imu.quaternion[i] = q;
    }
    if (quaternion_norm < 0.81 || quaternion_norm > 1.21)
        throw std::runtime_error("Invalid IMU quaternion norm");
    for (int i = 0; i < 3; ++i) {
        if (!std::isfinite(snapshot.imu_state().gyroscope()[i]) ||
            !std::isfinite(snapshot.imu_state().accelerometer()[i]))
            throw std::runtime_error("Invalid IMU feedback");
        state->imu.gyroscope[i] = snapshot.imu_state().gyroscope()[i];
        state->imu.accelerometer[i] = snapshot.imu_state().accelerometer()[i];
    }
    state->userCmd = cmdPanel->getUserCmd();
    state->userValue = cmdPanel->getUserValue();
    cmd_.crc() = crc32_core(reinterpret_cast<const uint32_t *>(&cmd_), (sizeof(cmd_) >> 2) - 1);
    publisher_->Write(cmd_);
}
