//
//  RenderingClient.h
//  gvr-interface/src
//
//  Created by Stephen Birarda on 1/20/15.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//


#ifndef hifi_RenderingClient_h
#define hifi_RenderingClient_h

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Client.h"

class RenderingClient : public Client {
    Q_OBJECT
public:
    RenderingClient(QObject* parent = 0);
    ~RenderingClient();
    
    const glm::vec3& getPosition() const { return _position; }
    const glm::quat& getOrientation() const { return _orientation; }
    
    static glm::vec3 getPositionForAudio() { return _instance->getPosition(); }
    static glm::quat getOrientationForAudio() { return _instance->getOrientation(); }
    
private slots:
    void goToLocation(const glm::vec3& newPosition,
                      bool hasOrientationChange, const glm::quat& newOrientation,
                      bool shouldFaceLocation);
private:
    virtual void processVerifiedPacket(const HifiSockAddr& senderSockAddr, const QByteArray& incomingPacket);
    
    static RenderingClient* _instance;
    
    glm::vec3 _position;
    glm::quat _orientation;
};

#endif // hifi_RenderingClient_h
