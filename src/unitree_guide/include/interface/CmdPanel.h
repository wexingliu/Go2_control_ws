/**********************************************************************
 Copyright (c) 2020-2023, Unitree Robotics.Co.Ltd. All rights reserved.
***********************************************************************/
#ifndef CMDPANEL_H
#define CMDPANEL_H

#include "common/enumClass.h"
#include <pthread.h>
#include <mutex>


struct UserValue{
    float lx;
    float ly;
    float rx;
    float ry;
    float L2;
    UserValue(){
        setZero();
    }
    void setZero(){
        lx = 0;
        ly = 0;
        rx = 0;
        ry = 0;
        L2 = 0;
    }
};

class CmdPanel{
public:
    CmdPanel(){}
    virtual ~CmdPanel(){}
    UserCommand getUserCmd(){std::lock_guard<std::mutex> lock(mutex_); return userCmd;}
    UserValue getUserValue(){std::lock_guard<std::mutex> lock(mutex_); return userValue;}
    void setPassive(){std::lock_guard<std::mutex> lock(mutex_); userCmd = UserCommand::L2_B;}
    void setZero(){std::lock_guard<std::mutex> lock(mutex_); userValue.setZero();}
protected:
    std::mutex mutex_;
    virtual void* run(void *arg){return NULL;}
    UserCommand userCmd;
    UserValue userValue;
};

#endif  // CMDPANEL_H
