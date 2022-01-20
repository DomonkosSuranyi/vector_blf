// SPDX-FileCopyrightText: 2013-2021 Tobias Lorenz <tobias.lorenz@gmx.net>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "platform.h"

#include "AbstractFile.h"
#include "AfdxLineStatus.h"
#include "ObjectHeader.h"

#include "vector_blf_export.h"

namespace Vector {
namespace BLF {

/**
 * @brief AFDX_STATUS
 *
 * AFDX adapter status event, available since 8.2
 */
struct VECTOR_BLF_EXPORT AfdxStatus final : ObjectHeader {
    AfdxStatus();

    void read(AbstractFile & is) override;
    void write(AbstractFile & os) override;
    uint32_t calculateObjectSize() const override;

    /**
     * @brief application channel
     */
    uint16_t channel {};

    /** reserved */
    uint16_t reservedAfdxStatus1 {};

    /**
     * @brief status of adapter lineA
     */
    AfdxLineStatus statusA {};

    /**
     * @brief status of adapter lineB
     */
    AfdxLineStatus statusB {};

    /** reserved */
    uint32_t reservedAfdxStatus2 {};
};

}
}
