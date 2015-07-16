//
//  PacketReceiver.h
//  libraries/networking/src
//
//  Created by Stephen Birarda on 1/23/2014.
//  Update by Ryan Huffman on 7/8/2015.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_PacketReceiver_h
#define hifi_PacketReceiver_h

#include <QtCore/QMap>
#include <QtCore/QMetaMethod>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QSet>

#include "NLPacket.h"
#include "udt/PacketHeaders.h"

class OctreePacketProcessor;
class PacketListener;

class PacketReceiver : public QObject {
    Q_OBJECT
public:
    PacketReceiver(QObject* parent = 0);
    PacketReceiver(const PacketReceiver&) = delete;

    PacketReceiver& operator=(const PacketReceiver&) = delete;
    
    int getInPacketCount() const { return _inPacketCount; }
    int getInByteCount() const { return _inByteCount; }

    void setShouldDropPackets(bool shouldDropPackets) { _shouldDropPackets = shouldDropPackets; }
    
    void resetCounters() { _inPacketCount = 0; _inByteCount = 0; }

    bool registerListenerForTypes(const QSet<PacketType::Value>& types, PacketListener* listener, const char* slot);
    bool registerListener(PacketType::Value type, PacketListener* listener, const char* slot);
    void unregisterListener(PacketListener* listener);

public slots:
    void processDatagrams();

signals:
    void dataSent(quint8 channelType, int bytes);
    void dataReceived(quint8 channelType, int bytes);
    void packetVersionMismatch(PacketType::Value type);
    
private:
    // this is a brutal hack for now - ideally GenericThread / ReceivedPacketProcessor
    // should be changed to have a true event loop and be able to handle our QMetaMethod::invoke
    void registerDirectListenerForTypes(const QSet<PacketType::Value>& types, PacketListener* listener, const char* slot);
    
    bool packetVersionMatch(const NLPacket& packet);

    QMetaMethod matchingMethodForListener(PacketType::Value type, QObject* object, const char* slot) const;
    void registerVerifiedListener(PacketType::Value type, QObject* listener, const QMetaMethod& slot);
    
    using ObjectMethodPair = std::pair<QObject*, QMetaMethod>;

    QMutex _packetListenerLock;
    QHash<PacketType::Value, ObjectMethodPair> _packetListenerMap;
    int _inPacketCount = 0;
    int _inByteCount = 0;
    bool _shouldDropPackets = false;
    QMutex _directConnectSetMutex;
    QSet<QObject*> _directlyConnectedObjects;
    
    friend class OctreePacketProcessor;
};

#endif // hifi_PacketReceiver_h
