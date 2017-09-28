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

#include "cppWebSockets/Util.h"
#include "cppWebSockets/WebSocketServer.h"
#include "dataserver.h"


//------------------------------------------------------------------------------------------------------
void DataServer::onConnect( int socketID )
{
    // Give this connection a random user ID
    const string& handle = "User #" + Util::toString( socketID );
    Util::log( "New connection: " + handle );
    
    // Associate this handle with the connection
    this->setValue( socketID, "handle", handle );

    // Let everyone know the new user has connected
    this->broadcast( handle + " has connected." );
}

//------------------------------------------------------------------------------------------------------
void DataServer::onMessage( int socketID, const string& data )
{
    // Send the received message to all connected clients in the form of 'User XX: message...'
    Util::log( "Received: " + data );
    const string& message = this->getValue( socketID, "handle" ) + ": " + data;

    this->broadcast( message );
}

//------------------------------------------------------------------------------------------------------
void DataServer::sendString(const string& message)
{
    this->broadcast( message );
}

//------------------------------------------------------------------------------------------------------
void DataServer::onDisconnect( int socketID )
{
    const string& handle = this->getValue( socketID, "handle" );
    Util::log( "Disconnected: " + handle );
    
    // Let everyone know the user has disconnected
    const string& message = handle + " has disconnected.";
    for( map<int,Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
        if( it->first != socketID )
            // The disconnected connection gets deleted after this function runs, so don't try to send to it
            // (It's still around in case the implementing class wants to perform any clean up actions)
            this->send( it->first, message );
}

//------------------------------------------------------------------------------------------------------
void DataServer::onError( int socketID, const string& message )
{
    Util::log( "Error: " + message );
}

