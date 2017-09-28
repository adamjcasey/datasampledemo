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

#include <iostream>
#include <cstdint>

#include "dataserver.h"
#include "samples.h"

//------------------------------------------------------------------------------------------------------
//  Definitions
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    DataServer ds = DataServer(8080);
    Samples s;
    std::cout << "Chrysler Demo" << std::endl;

    while (1)
    {
        ds.wait(50);

        //---------------------------------------------------------------------------------------
        // Here is where you append all your samples.  This should take 1 second.
        // This is all faked here for illustration purposes
        s.Clear();

        while (!s.IsSamplingComplete())
        {
            Samples::DataFromHardware data;
            data.v1 = static_cast<uint16_t>((5.2/10.0) * 4095);
            data.v2 = static_cast<uint16_t>((6.4/10.0) * 4095);
            data.v3 = static_cast<uint16_t>((2.8/10.0) * 4095);
            data.i1 = static_cast<uint16_t>((8.2/10.0) * 4095);
            data.i2 = static_cast<uint16_t>((3.4/10.0) * 4095);
            data.i3 = static_cast<uint16_t>((1.8/10.0) * 4095);
            s.Append(data);
        }
        // This would take a second but we just faked it so sleep
        sleep(1);
        //----------------------------------------------------------------------------------------

        // Create the string
        double vavg1 = s.GetVoltageAverage(Samples::Phase::PHASE_1);    
        double vavg2 = s.GetVoltageAverage(Samples::Phase::PHASE_2);    
        double vavg3 = s.GetVoltageAverage(Samples::Phase::PHASE_3);    
        double irms1 = s.GetCurrentRMS(Samples::Phase::PHASE_1);    
        double irms2 = s.GetCurrentRMS(Samples::Phase::PHASE_2);    
        double irms3 = s.GetCurrentRMS(Samples::Phase::PHASE_3);    

        char buff[256];
        snprintf(buff, sizeof(buff), "{\"v1\":\"%f\",\"v2\":\"%f\",\"v3\":\"%f\",\"i1\":\"%f\",\"i2\":\"%f\",\"i3\":\"%f\"}", vavg1, vavg2, vavg3, irms1, irms2, irms3);

        ds.broadcast(buff);
    }
}
