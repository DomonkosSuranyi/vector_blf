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
 * @brief DATA_LOST_BEGIN
 */
struct VECTOR_BLF_EXPORT DataLostBegin final : ObjectHeader {
    DataLostBegin();

    void read(AbstractFile & is) override;
    void write(AbstractFile & os) override;
    uint32_t calculateObjectSize() const override;

    /** enumeration for queueIdentifier */
    enum QueueIdentifier : uint32_t {
        RtQueue = 0,
        AnlyzQueue = 1,
        RtAndAnlyzQueue = 2
    };

    /** identifier for the leaking queue */
    uint32_t queueIdentifier {};
};

}
}
