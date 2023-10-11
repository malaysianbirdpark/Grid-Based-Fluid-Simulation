#include "pch.h"
#include "Clk.h"

using namespace std::chrono;

void Clk::Init() {
    _checkPoint = high_resolution_clock::now();
}

float Clk::Mark() {
    auto const resrv{ _checkPoint };
    _checkPoint = high_resolution_clock::now();
    duration<float> const last_frame_time{ _checkPoint - resrv };
    return last_frame_time.count();
}

float Clk::Peek() {
    return duration<float>(high_resolution_clock::now() - _checkPoint).count();
}
