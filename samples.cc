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
#include <iostream>
#include <chrono>
#include <string>
#include <sstream>
#include <vector>

#include <sqlite3.h>
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

    // If this is the last sample, store in the database
    if (sample_counter_ < SAMPLING_FREQUENCY_HZ)
        return;

    // Insert data item into database
    InsertSampleIntoDatabase();
}

//------------------------------------------------------------------------------------------------------
std::string Samples::GetJSON()
{
    std::stringstream buffer;

    buffer << "{\"live\":" << GetJSONLive() << ",";
    buffer << "\"fifteenmins\":" << GetJSONHistory(1) << ",";
    buffer << "\"onehour\":" << GetJSONHistory(1) << ",";
    buffer << "\"tenhours\":" << GetJSONHistory(1) << ",";
    buffer << "\"twentyfourhours\":" << GetJSONHistory(1);
    buffer << "}"; 

    return buffer.str();
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

//------------------------------------------------------------------------------------------------------
void Samples::InsertSampleIntoDatabase()
{
    // Insert data item into database
    int rc;
    sqlite3 * db_;
    rc = sqlite3_open("chrysler.sqlite3", &db_);
    if (rc != SQLITE_OK)
    {
        std::cout << "InsertSampleIntoDatabase sqlite3_open failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return;
    }
    sqlite3_stmt *stmt;
    unsigned long timestamp = std::chrono::duration_cast<std::chrono::seconds>
                                        (std::chrono::system_clock::now().time_since_epoch()).count();
    rc = sqlite3_prepare_v2(db_, "INSERT INTO samples (unixtimestamp, v1, v2, v3, i1, i2, i3) values (?,?,?,?,?,?,?)", -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cout << "InsertSampleIntoDatabase sqlite3_prepare failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return;
    }

    // Bind the values
    sqlite3_bind_int(stmt, 1, timestamp);
    sqlite3_bind_double(stmt, 2, GetVoltageAverage(Phase::PHASE_1));
    sqlite3_bind_double(stmt, 3, GetVoltageAverage(Phase::PHASE_2));
    sqlite3_bind_double(stmt, 4, GetVoltageAverage(Phase::PHASE_3));
    sqlite3_bind_double(stmt, 5, GetCurrentRMS(Phase::PHASE_1));
    sqlite3_bind_double(stmt, 6, GetCurrentRMS(Phase::PHASE_2));
    sqlite3_bind_double(stmt, 7, GetCurrentRMS(Phase::PHASE_3));


    // Commit 
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cout << "InsertSampleIntoDatabase sqlite3_step failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return;
    }

    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK)
    {
        std::cout << "InsertSampleIntoDatabase sqlite3_finalize failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return;
    }

    sqlite3_close(db_);
}

//------------------------------------------------------------------------------------------------------
std::string Samples::GetJSONLive()
{
    // Get the sample from the database
    std::stringstream buffer;

    sqlite3 * db_;
    int rc = sqlite3_open("chrysler.sqlite3", &db_);
    if (rc != SQLITE_OK)
    {
        buffer << "GetJSONLive sqlite3_open failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return buffer.str();
    }

    sqlite3_stmt *stmt;    
    rc = sqlite3_prepare(db_, "SELECT * FROM samples ORDER BY unixtimestamp DESC LIMIT 1", -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        buffer << "GetJSONLive sqlite3_prepare failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return buffer.str();
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) 
    {
        buffer << "GetJSONLivesqlite3_step ROW failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return buffer.str();
    }

    // Create the string
    buffer.precision(2);
    buffer << "{";    
    buffer << "\"v1\":" << std::fixed << sqlite3_column_double(stmt, 1) << ",";    
    buffer << "\"v2\":" << std::fixed << sqlite3_column_double(stmt, 2) << ",";
    buffer << "\"v3\":" << std::fixed << sqlite3_column_double(stmt, 3) << ",";
    buffer << "\"i1\":" << std::fixed << sqlite3_column_double(stmt, 4) << ",";
    buffer << "\"i2\":" << std::fixed << sqlite3_column_double(stmt, 5) << ",";
    buffer << "\"i3\":" << std::fixed << sqlite3_column_double(stmt, 6);
    buffer << "}";    

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        buffer << "GetJSONLivesqlite3_step DONE failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return buffer.str();
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db_);

    return buffer.str();
}

//------------------------------------------------------------------------------------------------------
std::string Samples::GetJSONHistory(int decimation)
{
    // Get the sample from the database
    std::stringstream buffer;
    std::vector<double> v1;
    std::vector<double> v2;
    std::vector<double> v3;
    std::vector<double> i1;
    std::vector<double> i2;
    std::vector<double> i3;

    sqlite3 * db_;
    int rc = sqlite3_open("chrysler.sqlite3", &db_);
    if (rc != SQLITE_OK)
    {
        buffer << "GetJSONLive sqlite3_open failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return buffer.str();
    }

    sqlite3_stmt *stmt;    

    char query[128];
    sprintf(query, "SELECT * FROM samples WHERE ROWID %% %d = 0 ORDER BY unixtimestamp DESC LIMIT 40", decimation);

    rc = sqlite3_prepare(db_, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        buffer << "GetJSONLive sqlite3_prepare failed " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        return buffer.str();
    }

    while (1)
    {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) 
        {
            v1.push_back(sqlite3_column_double(stmt, 1));
            v2.push_back(sqlite3_column_double(stmt, 2));
            v3.push_back(sqlite3_column_double(stmt, 3));
            i1.push_back(sqlite3_column_double(stmt, 4));
            i2.push_back(sqlite3_column_double(stmt, 5));
            i3.push_back(sqlite3_column_double(stmt, 6));
        }
        else
            break;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db_);

    // Create the string
    buffer.precision(1);
    buffer << "{";
    // v1
    buffer << "\"v1\":[";     
    for (double& d: v1)
        buffer << std::fixed << d << ",";
    buffer << "0.0],";     
    // v2
    buffer << "\"v2\":[";     
    for (double& d: v2)
        buffer << std::fixed << d << ",";
    buffer << "0.0],";     
    // v3
    buffer << "\"v3\":[";     
    for (double& d: v3)
        buffer << std::fixed << d << ",";
    buffer << "0.0],";     
    // i1
    buffer << "\"i1\":[";     
    for (double& d: i1)
        buffer << std::fixed << d << ",";
    buffer << "0.0],";     
    // i2
    buffer << "\"i2\":[";     
    for (double& d: i2)
        buffer << std::fixed << d << ",";
    buffer << "0.0],";     
    // i3
    buffer << "\"i3\":[";     
    for (double& d: i3)
        buffer << std::fixed << d << ",";
    buffer << "0.0]}";     

    return buffer.str();
}
