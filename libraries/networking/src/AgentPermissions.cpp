//
//  AgentPermissions.cpp
//  libraries/networking/src/
//
//  Created by Seth Alves on 2016-6-1.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <QDataStream>
#include <QtCore/QDebug>
#include "AgentPermissions.h"

QString AgentPermissions::standardNameLocalhost = QString("localhost");
QString AgentPermissions::standardNameLoggedIn = QString("logged-in");
QString AgentPermissions::standardNameAnonymous = QString("anonymous");


AgentPermissions& AgentPermissions::operator|=(const AgentPermissions& rhs) {
    this->canConnectToDomain |= rhs.canConnectToDomain;
    this->canAdjustLocks |= rhs.canAdjustLocks;
    this->canRezPermanentEntities |= rhs.canRezPermanentEntities;
    this->canRezTemporaryEntities |= rhs.canRezTemporaryEntities;
    this->canWriteToAssetServer |= rhs.canWriteToAssetServer;
    this->canConnectPastMaxCapacity |= rhs.canConnectPastMaxCapacity;
    return *this;
}
AgentPermissions& AgentPermissions::operator|=(const AgentPermissionsPointer& rhs) {
    if (rhs) {
        *this |= *rhs.get();
    }
    return *this;
}
AgentPermissionsPointer& operator|=(AgentPermissionsPointer& lhs, const AgentPermissionsPointer& rhs) {
    if (lhs && rhs) {
        *lhs.get() |= rhs;
    }
    return lhs;
}


QDataStream& operator<<(QDataStream& out, const AgentPermissions& perms) {
    out << perms.canConnectToDomain;
    out << perms.canAdjustLocks;
    out << perms.canRezPermanentEntities;
    out << perms.canRezTemporaryEntities;
    out << perms.canWriteToAssetServer;
    out << perms.canConnectPastMaxCapacity;
    return out;
}

QDataStream& operator>>(QDataStream& in, AgentPermissions& perms) {
    in >> perms.canConnectToDomain;
    in >> perms.canAdjustLocks;
    in >> perms.canRezPermanentEntities;
    in >> perms.canRezTemporaryEntities;
    in >> perms.canWriteToAssetServer;
    in >> perms.canConnectPastMaxCapacity;
    return in;
}

QDebug operator<<(QDebug debug, const AgentPermissions& perms) {
    debug.nospace() << "[permissions: " << perms.getID() << " --";
    if (perms.canConnectToDomain) {
        debug << " connect";
    }
    if (perms.canAdjustLocks) {
        debug << " locks";
    }
    if (perms.canRezPermanentEntities) {
        debug << " rez";
    }
    if (perms.canRezTemporaryEntities) {
        debug << " rez-tmp";
    }
    if (perms.canWriteToAssetServer) {
        debug << " asset-server";
    }
    if (perms.canConnectPastMaxCapacity) {
        debug << " ignore-max-cap";
    }
    debug.nospace() << "]";
    return debug.nospace();
}
QDebug operator<<(QDebug debug, const AgentPermissionsPointer& perms) {
    if (perms) {
        return operator<<(debug, *perms.get());
    }
    debug.nospace() << "[permissions: null]";
    return debug.nospace();
}
