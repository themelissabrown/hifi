//
//  AnimVariantMap.cpp
//  library/animation
//
//  Created by Howard Stearns on 10/15/15.
//  Copyright (c) 2015 High Fidelity, Inc. All rights reserved.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <QScriptEngine>
#include <QScriptValueIterator>
#include <RegisteredMetaTypes.h>
#include "AnimVariant.h"

QScriptValue AnimVariantMap::animVariantMapToScriptValue(QScriptEngine* engine) const {
    QScriptValue target = engine->newObject();
    for (auto& pair : _map) {
        switch (pair.second.getType()) {
            case AnimVariant::Type::Bool:
                target.setProperty(pair.first, pair.second.getBool());
                break;
            case AnimVariant::Type::Int:
                target.setProperty(pair.first, pair.second.getInt());
                break;
            case AnimVariant::Type::Float:
                target.setProperty(pair.first, pair.second.getFloat());
                break;
            case AnimVariant::Type::String:
                target.setProperty(pair.first, pair.second.getString());
                break;
            case AnimVariant::Type::Vec3:
                target.setProperty(pair.first, vec3toScriptValue(engine, pair.second.getVec3()));
                break;
            case AnimVariant::Type::Quat:
                target.setProperty(pair.first, quatToScriptValue(engine, pair.second.getQuat()));
                break;
            default:
                // Note that we don't do mat4 in Javascript currently, and there's not yet a reason to start now.
                assert("AnimVariant::Type" == "valid");
        }
    }
    return target;
}
void AnimVariantMap::copyVariantsFrom(const AnimVariantMap& other) {
    for (auto& pair : other._map) {
        _map[pair.first] = pair.second;
    }
}
void AnimVariantMap::animVariantMapFromScriptValue(const QScriptValue& source) {
    // POTENTIAL OPTIMIZATION: cache the types we've seen. I.e, keep a dictionary mapping property names to an enumeration of types.
    // Whenever we identify a new outbound type in animVariantMapToScriptValue above, or a new inbound type in the code that follows here,
    // we would enter it into the dictionary. Then switch on that type here, with the code that follow being executed only if
    // the type is not known. One problem with that is that there is no checking that two different script use the same name differently.
    QScriptValueIterator property(source);
    // Note: QScriptValueIterator iterates only over source's own properties. It does not follow the prototype chain.
    while (property.hasNext()) {
        property.next();
        QScriptValue value = property.value();
        if (value.isBool()) {
            set(property.name(), value.toBool());
            continue;
        } else if (value.isString()) {
            set(property.name(), value.toString());
            continue;
        } else if (value.isNumber()) {
            int asInteger = value.toInt32();
            float asFloat = value.toNumber();
            if (asInteger == asFloat) {
                set(property.name(), asInteger);
            } else {
                set(property.name(), asFloat);
            }
            continue;
        } else if (value.isObject()) {
            QScriptValue x = value.property("x");
            if (x.isNumber()) {
                QScriptValue y = value.property("y");
                if (y.isNumber()) {
                    QScriptValue z = value.property("z");
                    if (z.isNumber()) {
                        QScriptValue w = value.property("w");
                        if (w.isNumber()) {
                            set(property.name(), glm::quat(x.toNumber(), y.toNumber(), z.toNumber(), w.toNumber()));
                        } else {
                            set(property.name(), glm::vec3(x.toNumber(), y.toNumber(), z.toNumber()));
                        }
                        continue;
                    }
                }
            }
        }
        qCWarning(animation) << "Ignoring unrecognized data" << value.toString() << "for animation property" << property.name();
     }
}
