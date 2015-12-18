//  arcBallEntityScript.js
//  
//  Script Type: Entity
//  Created by Eric Levin on 12/17/15.
//  Copyright 2015 High Fidelity, Inc.
//
//  This entity script handles the logic for the arcBall rave toy
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

(function() {
    Script.include("../../libraries/utils.js");
    var _this;
    var ArcBall = function() {
        _this = this;
    };

    ArcBall.prototype = {
        isGrabbed: false,
        startNearGrab: function() {
            //Search for nearby balls and create an arc to it if one is found
            var position = Entities.getEntityProperties(this.entityID, "position").position
            var entities = Entities.findEntities(position, 10);
            entities.forEach(function(entity) {
                var props = Entities.getEntityProperties(entity, ["position", "name"]);
                if (props.name === "Arc Ball" && JSON.stringify(_this.entityID) !== JSON.stringify(entity)) {
                    _this.createBeam(position, props.position);
                }
            });

        },

        createBeam: function(startPosition, endPosition) {
            print("CREATE BEAM")
                // Creates particle arc from start position to end position
            var rotation = Entities.getEntityProperties(this.entityID, "rotation").rotation;
            var sourceToTargetVec = Vec3.subtract(endPosition, startPosition);
            var emitOrientation = Quat.rotationBetween(Vec3.UNIT_Z, sourceToTargetVec);
            emitOrientation = Quat.multiply(Quat.inverse(rotation), emitOrientation);

            testBox = Entities.addEntity({
                type: "Box",
                dimensions: {x: .1, y: .1, z: 1},
                color: {red: 200, green: 10, blue: 10},
                position: startPosition,
                rotation: emitOrientation,
                visible: false
            });
            var color = {
                red: 200,
                green: 10,
                blue: 10
            };
            var props = {
                type: "ParticleEffect",
                name: "Particle Arc",
                parentID: this.entityID,
                parentJointIndex: -1,
                position: startPosition,
                isEmitting: true,
                colorStart: color,
                color: {
                    red: 200,
                    green: 200,
                    blue: 255
                },
                colorFinish: color,
                maxParticles: 100000,
                lifespan: 6,
                emitRate: 1000,
                emitOrientation: emitOrientation,
                emitSpeed: .4,
                speedSpread: 0.0,
                emitDimensions: {
                    x: 0,
                    y: 0,
                    z: 0
                },
                polarStart: 0,
                polarFinish: .0,
                azimuthStart: .1,
                azimuthFinish: .01,
                emitAcceleration: {
                    x: 0,
                    y: 0,
                    z: 0
                },
                accelerationSpread: {
                    x: .00,
                    y: .00,
                    z: .00
                },
                radiusStart: 0.03,
                adiusFinish: 0.025,
                alpha: 0.7,
                alphaSpread: .1,
                alphaStart: 0.5,
                alphaFinish: 0.5,
                textures: "https://s3.amazonaws.com/hifi-public/eric/textures/particleSprites/beamParticle.png",
                emitterShouldTrail: false
            }
            this.particleArc = Entities.addEntity(props);
        },

        continueNearGrab: function() {},

        releaseGrab: function() {},

        unload: function() {
            Entities.deleteEntity(this.particleArc);
            Entities.deleteEntity(testBox);
        },

        preload: function(entityID) {
            this.entityID = entityID;
        },
    };
    return new ArcBall();
});