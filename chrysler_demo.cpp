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
#include <math.h>


#include "dataserver.h"
#include "samples.h"

//------------------------------------------------------------------------------------------------------
//  Definitions
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    DataServer ds = DataServer(8088);
    Samples s;
    std::cout << "Chrysler Demo" << std::endl;

    while  (1)
    {
        ds.wait(50);

        //---------------------------------------------------------------------------------------
        // Here is where you append all your samples.  This should take 1 second.
        // This is all faked here for illustration purposes
        s.Clear();

        while (!s.IsSamplingComplete())
        {
            Samples::DataFromHardware data;
            data.v1 = static_cast<uint16_t>(((7.9 + (((double) rand() / (RAND_MAX)) / 10.0) ) / 10.0) * 4095);
            data.v2 = static_cast<uint16_t>(((7.9 + (((double) rand() / (RAND_MAX)) / 10.0) ) / 10.0) * 4095);
            data.v3 = static_cast<uint16_t>(((7.9 + (((double) rand() / (RAND_MAX)) / 10.0) ) / 10.0) * 4095);
            data.i1 = static_cast<uint16_t>(((7.1 + (((double) rand() / (RAND_MAX)) / 10.0) ) / 10.0) * 4095);
            data.i2 = static_cast<uint16_t>(((7.1 + (((double) rand() / (RAND_MAX)) / 10.0) ) / 10.0) * 4095);
            data.i3 = static_cast<uint16_t>(((7.1 + (((double) rand() / (RAND_MAX)) / 10.0) ) / 10.0) * 4095);
            s.Append(data);
        }
        // This would take a second but we just faked it so sleep
        sleep(1);

        // Push the data to the database

        //----------------------------------------------------------------------------------------

        // Create the string
        std::string json = s.GetJSON();
        ds.broadcast(json.c_str());
    }
}
