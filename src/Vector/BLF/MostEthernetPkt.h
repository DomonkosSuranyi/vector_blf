/*
 * Copyright (C) 2013-2020 Tobias Lorenz.
 * Contact: tobias.lorenz@gmx.net
 *
 * This file is part of Tobias Lorenz's Toolkit.
 *
 * Commercial License Usage
 * Licensees holding valid commercial licenses may use this file in
 * accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Tobias Lorenz.
 *
 * GNU General Public License 3.0 Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 */

#pragma once

#include <Vector/BLF/platform.h>

#include <vector>

#include <Vector/BLF/AbstractFile.h>
#include <Vector/BLF/ObjectHeader2.h>
#include <Vector/BLF/VectorTypes.h>

#include <Vector/BLF/vector_blf_export.h>

namespace Vector {
namespace BLF {

/**
 * @brief MOST_ETHERNET_PKT
 *
 * Message on MOST Ethernet Packet Channel.
 */
struct VECTOR_BLF_EXPORT MostEthernetPkt final : ObjectHeader2 {
    MostEthernetPkt();

    void read(AbstractFile & is) override;
    void write(AbstractFile & os) override;
    DWORD calculateObjectSize() const override;

    /**
     * @brief application channel
     *
     * Application channel
     */
    WORD channel {};

    /**
     * @brief direction: 0: Rx; 1: Tx; 2: TxRequest
     *
     * Direction of message events:
     *   - 0: Rx (received)
     *   - 1: Tx (transmit receipt)
     *   - 2: Tx Request (transmit request)
     */
    BYTE dir {};

    /** reserved */
    BYTE reservedMostEthernetPkt1 {};

    /** reserved */
    DWORD reservedMostEthernetPkt2 {};

    /**
     * @brief 48 bit source address
     *
     * 48 bit source address
     */
    ULONGLONG sourceMacAdr {};

    /**
     * @brief 48 bit target address
     *
     * 48 bit target address
     */
    ULONGLONG destMacAdr {};

    /**
     * @brief Tranfer Type
     *
     * Message-like events can either be recorded through the MOST transceiver chip or through a
     * separate network spy.
     *   - 1: Node
     *     MOST transceiver reported the message (either due to a successful reception or
     *     as acknowledgment for a transmit request).
     *   - 2: Spy
     *     Message was reported by the network spy. The Spy sees all messages
     *     independently of the desti-nation address.
     */
    BYTE transferType {};

    /**
     * @brief transmission status
     *
     * Transmission states of MOST50/150 messages and packets.
     *   - Bit 0:
     *     Meaning:
     *       - 0: bus inactive
     *       - 1: bus active
     *     Restriction:
     *       - only for Dir = Rx or mTransferType = Spy
     *   - Bit 4:
     *     Meaning:
     *       - 1: acknowledged (Ack)
     *     Restriction
     *       - only for mTransferType = Node and Dir =
     *         Tx (always set to 1 for Rx messages in node
     *         mode)
     *   - Bit 5:
     *     Meaning:
     *       - 1: not acknowledged (NAck)
     *     Restriction:
     *       - only for Dir = Tx
     *   - Bit 6:
     *     Meaning: Send result:
     *       - 0: Transmission error (TxF)
     *       - 1: OK
     *     Restriction:
     *       - only for Dir = Tx
     *       - only for mTransferType = Node
     */
    BYTE state {};

    /**
     * @brief acknowledge code
     *
     * AckNack holds the transmit status of a control message (see Transmit Status Register of OS8104
     * for MOST25).
     *   - Bit 0:
     *     Meaning:
     *       - 1: no response (NoResp)
     *     Restriction:
     *       - only for Dir = Tx or spy messages
     *   - Bit 1:
     *     Meaning:
     *       - 1: valid receipt (Valid)
     *     Restriction:
     *       - only for Dir = Tx or spy messages
     *   - Bit 2:
     *     Meaning:
     *       - 1: CRC Error (CRCError)
     *     Restriction:
     *       - only for Dir = Tx or spy messages
     *   - Bit 3:
     *     Meaning:
     *       - 1: receive buffer full (RxBufFull)
     *     Restriction:
     *       - only for Dir = Tx or spy messages
     *   - Bit 4:
     *     Meaning:
     *       - 1: acknowledged (Ack)
     *     Restriction:
     *       - only for Dir = Tx or spy messages (always
     *         set to 1 for Rx messages in node mode)
     *   - Bit 5:
     *     Meaning:
     *       - 1: negative acknowledge (NAck)
     *     Restriction:
     *       - only for Dir = Tx or spy messages
     */
    BYTE ackNack {};

    /** reserved */
    BYTE reservedMostEthernetPkt3 {};

    /**
     * @brief Cyclic Redundancy Check
     *
     * Cyclic Redundancy Check
     */
    DWORD crc {};

    /**
     * @brief a preemptive acknowledge code
     *
     * Pre-emptive acknowledge code (spy only)
     *   - 0x00: No Response
     *   - 0x01: Buffer full
     *   - 0x04: OK
     */
    BYTE pAck {};

    /**
     * @brief CRC acknowledge from the packet receiver(s) to the packet transmitter
     *
     * CRC acknowledge from the packet receiver(s) to
     * the packet transmitter (spy only)
     *   - 0x00: No Response
     *   - 0x01: CRC error
     *   - 0x04: OK
     */
    BYTE cAck {};

    /** reserved */
    WORD reservedMostEthernetPkt4 {};

    /**
     * @brief length of variable data in bytes (1506 max)
     *
     * Length of variable data in bytes (1506 max)
     */
    DWORD pktDataLength {};

    /** reserved */
    ULONGLONG reservedMostEthernetPkt5 {};

    /**
     * @brief variable data
     *
     * Variable data
     */
    std::vector<uint8_t> pktData {};
};

}
}
