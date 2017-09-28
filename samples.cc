//------------------------------------------------------------------------------------------------------
//  Copyright © 2017 OXYS

// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

//   a. Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//   b. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   c. Neither the name of Continuum nor the names of its contributors
//      may be used to endorse or promote products derived from this software
//      without specific prior written permission.


// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.

// Created by Adam Casey 2017
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------------------------------

#include <math.h>
#include "samples.h"

//------------------------------------------------------------------------------------------------------
void Samples::Append(DataFromHardware data)
{
    if (sample_counter_ >= SAMPLING_FREQUENCY_HZ)
        return;

    voltage[Phase::PHASE_1][sample_counter_] = (static_cast<double>(data.v1) * SAMPLE_MAX_VOLTAGE / MAX_VALUE) * voltage_scale_;
    voltage[Phase::PHASE_2][sample_counter_] = (static_cast<double>(data.v2) * SAMPLE_MAX_VOLTAGE / MAX_VALUE) * voltage_scale_;
    voltage[Phase::PHASE_3][sample_counter_] = (static_cast<double>(data.v3) * SAMPLE_MAX_VOLTAGE / MAX_VALUE) * voltage_scale_;
    current[Phase::PHASE_1][sample_counter_] = ((static_cast<double>(data.i1) * SAMPLE_MAX_VOLTAGE / MAX_VALUE) - current_offset_) * current_scale_;
    current[Phase::PHASE_2][sample_counter_] = ((static_cast<double>(data.i2) * SAMPLE_MAX_VOLTAGE / MAX_VALUE) - current_offset_) * current_scale_;
    current[Phase::PHASE_3][sample_counter_] = ((static_cast<double>(data.i3) * SAMPLE_MAX_VOLTAGE / MAX_VALUE) - current_offset_) * current_scale_;

    sample_counter_++;
}

//------------------------------------------------------------------------------------------------------
double Samples::GetVoltageAverage(Phase p)
{
    double total = 0.0;
    for (int i = 0; i < SAMPLING_FREQUENCY_HZ; i++)
        total += voltage[p][i];

    return total / SAMPLING_FREQUENCY_HZ;
}

//------------------------------------------------------------------------------------------------------
double Samples::GetCurrentRMS(Phase p)
{
    double total = 0.0;
    for (int i = 0; i < SAMPLING_FREQUENCY_HZ; i++)
        total += pow(current[p][i], 2);

    return sqrt(total / SAMPLING_FREQUENCY_HZ);
}
