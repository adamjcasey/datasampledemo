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

#include <cstdint>
#include <string>

//------------------------------------------------------------------------------------------------------
class Samples
{
 public: 
        Samples() : sample_counter_(0), voltage_scale_(60.0), current_scale_(125.0/5.0), current_offset_(5.0) {}

        enum Phase
        {
            PHASE_1 = 0,
            PHASE_2 = 1,
            PHASE_3 = 2
        };

        struct DataFromHardware
        {
            uint16_t v1;
            uint16_t v2;
            uint16_t v3;
            uint16_t i1;
            uint16_t i2;
            uint16_t i3;
        };
    
        void Clear() { sample_counter_ = 0; }
        bool IsSamplingComplete() { return (sample_counter_ >= SAMPLING_FREQUENCY_HZ); }
        void Append(DataFromHardware data);
        std::string GetJSON();

 private:
        double GetVoltageAverage(Phase phase);
        double GetCurrentRMS(Phase phase);
        void InsertSampleIntoDatabase();
        std::string GetJSONLive();
        std::string GetJSONHistory(int decimation);

        static constexpr int SAMPLING_FREQUENCY_HZ = 250;
        static constexpr int NUM_PHASES = 3;
        static constexpr double SAMPLE_MAX_VOLTAGE = 10.0;
        static constexpr int NUMBER_OF_SAMPLE_BITS = 12;
        static constexpr double MAX_VALUE = 4095;

        double voltage_scale_;
        double current_scale_;
        double current_offset_;
        uint16_t sample_counter_;
        double voltage[NUM_PHASES][SAMPLING_FREQUENCY_HZ];
        double current[NUM_PHASES][SAMPLING_FREQUENCY_HZ];
};
