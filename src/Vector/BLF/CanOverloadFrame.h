// SPDX-FileCopyrightText: 2013-2021 Tobias Lorenz <tobias.lorenz@gmx.net>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "platform.h"

#include "AbstractFile.h"
#include "ObjectHeader.h"

#include "vector_blf_export.h"

namespace Vector {
namespace BLF {

/**
 * @brief CAN_OVERLOAD
 *
 * CAN overload frame received or transmitted on a CAN channel.
 */
struct VECTOR_BLF_EXPORT CanOverloadFrame final : ObjectHeader {
    CanOverloadFrame();

    void read(AbstractFile & is) override;
    void write(AbstractFile & os) override;
    uint32_t calculateObjectSize() const override;

    /**
     * @brief application channel
     *
     * Channel the frame was sent or received.
     */
    uint16_t channel {};

    /** reserved */
    uint16_t reservedCanOverloadFrame1 {};

    /** reserved */
    uint32_t reservedCanOverloadFrame2 {};
};

}
}
