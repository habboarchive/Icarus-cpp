#include "stdafx.h"

#include "game/session/Session.h"
#include "game/session/SessionDetails.h"
#include "boot/Icarus.h"

/*
    Session constructor

    @param NetworkConnection ptr
    @return session instance
*/
Session::Session(NetworkConnection *network_connection) : 
    network_connection(network_connection), session_details(nullptr) {
    
    std::cout << " [SESSION] Client connected with ID: " << this->getNetworkConnection()->getConnectionId() << std::endl;
}

/*
    Initialise player and load their data

    @return none
*/
void Session::login() {

    // Load player rooms
    Icarus::getGame()->getRoomManager()->createPlayerRooms(this->session_details->getId());
}

/*
    Send message composer to the session's socket

    @param MessageComposer ptr
    @return none
*/
void Session::send(MessageComposer &composer) {
    this->network_connection->send(composer);
}

/*
    Clear session of any responsibilities

    @return none
*/
void Session::clear() {
     
    // Can't continue clearing if session details is null...
    if (session_details == nullptr) {
        return;
    }

    // Dispose player rooms
    std::vector<Room*> rooms = Icarus::getGame()->getRoomManager()->getPlayerRooms(this->session_details->getId()); {
        for (Room *room : rooms) {
            room->dispose();
        }
    }
}

/*
    Session deconstructor, called when session disconnects, 
    is deleted from SessionManager class

    networkManager->connectionState has already been set to false 
    so there won't be any more packet receiving
*/
Session::~Session() {
    std::cout << " [SESSION] Client disconnected with ID: " << this->getNetworkConnection()->getConnectionId() << std::endl;

    if (session_details != nullptr) {
        delete session_details;
    }
}
