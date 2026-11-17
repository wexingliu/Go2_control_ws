#include <iostream>
#include <iomanip>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <sched.h>
#include <unistd.h>
#include "control/ControlFrame.h"
#include "control/CtrlComponents.h"
#include "interface/IODDS.h"

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    try {
        auto node = std::make_shared<rclcpp::Node>("unitree_guide");
        const auto interface = node->declare_parameter<std::string>("network_interface", "enp5s0");
        const auto domain = node->declare_parameter<int>("domain", 0);
        if (domain < 0 || domain > 232 || interface.empty())
            throw std::runtime_error("Invalid DDS domain or network interface");
        sched_param param{};
        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
        if (sched_setscheduler(getpid(), SCHED_FIFO, &param) == -1)
            std::cerr << "Real-time scheduling unavailable; continuing with normal scheduling" << std::endl;
        std::cout << std::fixed << std::setprecision(3);
        std::unique_ptr<IODDS> io(new IODDS(interface, domain));
        io->waitForState();
        std::unique_ptr<CtrlComponents> components(new CtrlComponents(io.release()));
        bool running = true;
        components->dt = 0.002;
        components->running = &running;
        components->robotModel = new Go2Robot();
        components->waveGen = new WaveGenerator(0.45, 0.5, Vec4(0, 0.5, 0.5, 0));
        components->geneObj();
        ControlFrame frame(components.get());
        while (rclcpp::ok()) frame.run();
        rclcpp::shutdown();
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "unitree_guide: " << e.what() << std::endl;
        rclcpp::shutdown();
        return 1;
    }
}
