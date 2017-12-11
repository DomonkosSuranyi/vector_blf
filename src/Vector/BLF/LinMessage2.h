/*
 * Copyright (C) 2013 Tobias Lorenz.
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

#include <array>

#include <Vector/BLF/AbstractFile.h>
#include <Vector/BLF/LinDatabyteTimestampEvent.h>
#include <Vector/BLF/ObjectHeader.h>
#include <Vector/BLF/VectorTypes.h>

#include <Vector/BLF/vector_blf_export.h>

namespace Vector {
namespace BLF {

/**
 * @brief LIN_MESSAGE2
 *
 * LIN frame received or transmitted on a LIN channel.
 */
class VECTOR_BLF_EXPORT LinMessage2 final : public ObjectHeader, public LinDatabyteTimestampEvent
{
public:
    LinMessage2();

    virtual void read(AbstractFile & is) override;
    virtual void write(AbstractFile & os) override;
    virtual DWORD calculateObjectSize() const override;

    /**
     * @brief data bytes
     *
     * Databyte values
     */
    std::array<BYTE, 8> data;

    /**
     * @brief checksum byte
     *
     * Checksum byte value
     */
    WORD crc;

    /**
     * @brief direction
     *
     * Direction of bus events
     *   - 0: Rx (received)
     *   - 1: Tx (transmit receipt)
     *   - 2: Tx Request (transmit request)
     */
    BYTE dir;

    /**
     * @brief simulated frame
     *
     * Flag indicating whether this frame a
     * simulated one:
     *   - 0: real frame
     *   - 1: simulated frame
     */
    BYTE simulated;

    /**
     * @brief Event-triggered frame
     *
     * Flag indicating whether this frame is
     * Event-Triggered one:
     *   - 0: not ETF
     *   - 1: ETF
     */
    BYTE isEtf;

    /**
     * @brief Unconditional frame associated with ETF - serial index
     *
     * Event-Triggered frame only: Index
     * of associated frame, which data is
     * carried
     */
    BYTE etfAssocIndex;

    /**
     * @brief Unconditional frame associated with ETF - id of ETF
     *
     * Event-Triggered frame only: Frame
     * identifier (6-bit) of associated frame,
     * which data is carried
     */
    BYTE etfAssocEtfId;

    /**
     * Slave Identifier in the Final State
     * Machine (obsolete)
     */
    BYTE fsmId;

    /**
     * State Identifier of a Slave in the
     * Final State Machine (obsolete)
     */
    BYTE fsmState;

    /** reserved */
    std::array<BYTE, 3> reserved;

    /* the following variables are only available in Version 2 and above */

    /**
     * @brief Response baudrate of the event in bit/sec
     *
     * Event's baudrate
     * measured in response [in
     * bits/sec]
     */
    DWORD respBaudrate;

    /* the following variables are only available in Version 3 and above */

    /**
     * @brief Exact baudrate of the header in bit/sec
     *
     * Event’s baudrate
     * measured in header [in
     * bits/sec]
     */
    DOUBLE exactHeaderBaudrate;

    /**
     * @brief Early stop bit offset for UART timestamps in ns
     *
     * Early stop bit offset in
     * frame header for UART
     * timestamps [in ns]
     */
    DWORD earlyStopbitOffset;

    /**
     * @brief Early stop bit offset in frame response for UART timestamps in ns
     *
     * Early stop bit offset in
     * frame response for
     * UART timestamps [in ns]
     */
    DWORD earlyStopbitOffsetResponse;
};

}
}
