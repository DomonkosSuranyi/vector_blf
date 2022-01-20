// SPDX-FileCopyrightText: 2013-2021 Tobias Lorenz <tobias.lorenz@gmx.net>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "platform.h"

#include <string>

#include "AbstractFile.h"
#include "ObjectHeader.h"

#include "vector_blf_export.h"

namespace Vector {
namespace BLF {

/**
 * @brief EVENT_COMMENT
 *
 * Comment of an event. The comment can be set in Trace Window.
 */
struct VECTOR_BLF_EXPORT EventComment final : ObjectHeader {
    EventComment();

    void read(AbstractFile & is) override;
    void write(AbstractFile & os) override;
    uint32_t calculateObjectSize() const override;

    /**
     * @brief commented event type
     *
     * Type of the commented event
     */
    uint32_t commentedEventType {};

    /**
     * @brief text length in bytes
     *
     * Length of text without ending 0.
     */
    uint32_t textLength {};

    /**
     * reserved
     */
    uint64_t reservedEventComment {};

    /**
     * @brief text in MBCS
     *
     * Comment text.
     */
    std::string text {};
};

}
}
