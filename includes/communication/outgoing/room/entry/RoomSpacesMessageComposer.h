#pragma once

#include "game/room/Room.h"
#include "game/player/Player.h"

#include "communication/outgoing/MessageComposer.h"

class RoomSpacesMessageComposer : public MessageComposer {

public:
    RoomSpacesMessageComposer(std::string space, std::string data)
        : space(space), data(data) { }

    Response compose() {
        Response response = this->createResponse();
        response.writeString(this->space);
        response.writeString(this->data);
        return response;
    }

    int getHeader() {
        return Outgoing::RoomSpacesMessageComposer;
    }

private:
    std::string space;
    std::string data;
};