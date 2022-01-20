// SPDX-FileCopyrightText: 2013-2021 Tobias Lorenz <tobias.lorenz@gmx.net>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "platform.h"

#include <array>
#include <vector>

#include "AbstractFile.h"
#include "CompactSerialEvent.h"
#include "GeneralSerialEvent.h"
#include "ObjectHeader.h"
#include "SingleByteSerialEvent.h"

#include "vector_blf_export.h"

namespace Vector {
namespace BLF {

/**
 * @brief SERIAL_EVENT
 *
 * serial event
 */
struct VECTOR_BLF_EXPORT SerialEvent final : ObjectHeader {
    SerialEvent();
    virtual ~SerialEvent() noexcept = default;
    SerialEvent(const SerialEvent &) = default;
    SerialEvent & operator=(const SerialEvent &) = default;
    SerialEvent(SerialEvent &&) = default;
    SerialEvent & operator=(SerialEvent &&) = default;

    void read(AbstractFile & is) override;
    void write(AbstractFile & os) override;
    uint32_t calculateObjectSize() const override;

    /** enumeration for flags */
    enum Flags : uint32_t {
        /** K-Line event */
        KLineEvent = 0x00000001,

        /**
         * diagnostic request
         *
         * only valid if KLineEvent is set
         */
        DiagRequest = 0x00000002,

        /**
         * single byte
         *
         * optimization for logging single bytes
         */
        SingleByte = 0x00000004,

        /**
         * compact byte
         *
         * optimization for logging a few bytes without additional timestamps
         */
        CompactByte = 0x00000008
    };

    /** flags */
    uint32_t flags {};

    /**
     * @brief channel of event
     *
     * channel of event
     */
    uint32_t port {};

    /**
     * @brief baudrate at which this event was transmitted (optional)
     *
     * baudrate at which this event was transmitted (optional)
     */
    uint32_t baudrate {};

    /** reserved */
    uint32_t reservedSerialEvent {};

    /** general serial event */
    GeneralSerialEvent general {};

    /** single byte serial event */
    SingleByteSerialEvent singleByte {};

    /** compact serial event */
    CompactSerialEvent compact {};
};

}
}
