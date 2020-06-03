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

#include <Vector/BLF/UncompressedFile.h>

#include <algorithm>
#include <cstring>

#include <Vector/BLF/Exceptions.h>

namespace Vector {
namespace BLF {

UncompressedFile::~UncompressedFile() {
    abort();
}

bool UncompressedFile::good() const {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    return (m_rdstate == std::ios_base::goodbit);
}

bool UncompressedFile::eof() const {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    return (m_rdstate & std::ios_base::eofbit);
}

std::streamsize UncompressedFile::gcount() const {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_gcount;
}

void UncompressedFile::read(char * s, std::streamsize n) {
    /* mutex lock */
    std::unique_lock<std::mutex> lock(m_mutex);

    /* wait until there is sufficient data */
    tellpChanged.wait(lock, [&] {
        return
        m_abort ||
        (n + m_tellg <= m_tellp) ||
        (n + m_tellg > m_fileSize);
    });

    /* handle read behind eof */
    if (n + m_tellg > m_fileSize) {
        n = m_fileSize - m_tellg;
        m_rdstate = std::ios_base::eofbit | std::ios_base::failbit;
    } else
        m_rdstate = std::ios_base::goodbit;

    /* read data */
    m_gcount = 0;
    while (n > 0) {
        /* find starting log container */
        std::shared_ptr<LogContainer> logContainer = logContainerContaining(m_tellg);
        if (!logContainer)
            break;

        /* offset to read */
        std::streamoff offset = m_tellg - logContainer->filePosition;

        /* copy data */
        std::streamsize gcount = std::min(n, static_cast<std::streamsize>(logContainer->uncompressedFileSize - offset));
        std::copy(logContainer->uncompressedFile.cbegin() + offset, logContainer->uncompressedFile.cbegin() + offset + gcount, s);

        /* remember get count */
        m_gcount += gcount;

        /* new get position */
        m_tellg += gcount;

        /* advance */
        s += gcount;

        /* calculate remaining data to copy */
        n -= gcount;
    }

    /* notify */
    tellgChanged.notify_all();
}

std::streampos UncompressedFile::tellg() {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* in case of failure return -1 */
    if (m_rdstate & (std::ios_base::failbit | std::ios_base::badbit))
        return -1;
    return m_tellg;
}

void UncompressedFile::seekg(const std::streampos pos) {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* new get position */
    m_tellg = pos;

    /* notify */
    tellgChanged.notify_all();
}

void UncompressedFile::seekg(const std::streamoff off, const std::ios_base::seekdir way) {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* new get position */
    switch(way) {
    case std::ios_base::beg:
        m_tellg = off;
        break;
    case std::ios_base::cur:
        m_tellg = m_tellg + off;
        break;
    case std::ios_base::end:
        m_tellg = m_fileSize + off;
        break;
    }

    /* notify */
    tellgChanged.notify_all();
}

void UncompressedFile::write(const char * s, std::streamsize n) {
    /* mutex lock */
    std::unique_lock<std::mutex> lock(m_mutex);

    /* wait for free space */
    tellgChanged.wait(lock, [&] {
        return
        m_abort ||
        ((m_tellp - m_tellg) < m_bufferSize);
    });

    /* write data */
    while (n > 0) {
        /* find starting log container */
        std::shared_ptr<LogContainer> logContainer = logContainerContaining(m_tellp);

        /* append new log container */
        if (!logContainer) {
            /* append new log container */
            logContainer = std::make_shared<LogContainer>();
            logContainer->uncompressedFile.resize(m_defaultLogContainerSize);
            logContainer->uncompressedFileSize = logContainer->uncompressedFile.size();
            if (!m_data.empty()) {
                logContainer->filePosition =
                    m_data.back()->uncompressedFileSize +
                    m_data.back()->filePosition;
            }
            m_data.push_back(logContainer);
        }

        /* offset to write */
        std::streamoff offset = m_tellp - logContainer->filePosition;

        /* copy data */
        std::streamsize pcount = std::min(n, static_cast<std::streamsize>(logContainer->uncompressedFileSize - offset));
        if (pcount > 0) {
            std::copy(s, s + pcount, logContainer->uncompressedFile.begin() + offset);

            /* new put position */
            m_tellp += pcount;

            /* advance */
            s += pcount;

            /* calculate remaining data to copy */
            n -= pcount;
        }
    }

    /* if new position is behind eof, shift it */
    if (m_tellp >= m_fileSize)
        m_fileSize = m_tellp;

    /* notify */
    tellpChanged.notify_all();
}

std::streampos UncompressedFile::tellp() {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* in case of failure return -1 */
    if (m_rdstate & (std::ios_base::failbit | std::ios_base::badbit))
        return -1;
    return m_tellp;
}

void UncompressedFile::seekp(const std::streampos pos) {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* new put position */
    m_tellp = pos;

    /* notify */
    tellpChanged.notify_all();
}

void UncompressedFile::seekp(const std::streamoff off, const std::ios_base::seekdir way) {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* new put position */
    switch(way) {
    case std::ios_base::beg:
        m_tellp = off;
        break;
    case std::ios_base::cur:
        m_tellp = m_tellp + off;
        break;
    case std::ios_base::end:
        m_tellp = m_fileSize + off;
        break;
    }

    /* notify */
    tellpChanged.notify_all();
}

void UncompressedFile::abort() {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* stop */
    m_abort = true;

    /* trigger blocked threads */
    tellgChanged.notify_all();
    tellpChanged.notify_all();
}

void UncompressedFile::write(const std::shared_ptr<LogContainer> & logContainer) {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* append logContainer */
    m_data.push_back(logContainer);
    logContainer->filePosition = m_tellp;

    /* advance put pointer */
    m_tellp += logContainer->uncompressedFileSize;

    /* notify */
    tellpChanged.notify_all();
}

void UncompressedFile::nextLogContainer() {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* find starting log container */
    std::shared_ptr<LogContainer> logContainer = logContainerContaining(m_tellp);
    if (logContainer) {
        /* offset to write */
        std::streamoff offset = m_tellp - logContainer->filePosition;

        /* resize logContainer, if it's not already a newly created one */
        if (offset > 0) {
            logContainer->uncompressedFile.resize(offset);
            logContainer->uncompressedFileSize = offset;
        }
    }
}

std::streamsize UncompressedFile::fileSize() const {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* return file size */
    return m_fileSize;
}

void UncompressedFile::setFileSize(std::streamsize fileSize) {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* set eof at m_dataEnd */
    m_fileSize = fileSize;

    /* notify */
    tellpChanged.notify_all();
}

void UncompressedFile::setBufferSize(std::streamsize bufferSize) {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* set max size */
    m_bufferSize = bufferSize;
}

void UncompressedFile::dropOldData() {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* check if drop should be done now */
    if (m_data.empty()) {
        return;
    }
    std::shared_ptr<LogContainer> logContainer = m_data.front();
    if (logContainer) {
        std::streampos position = logContainer->uncompressedFileSize + logContainer->filePosition;
        if ((position > m_tellg) || (position > m_tellp) || (position > m_fileSize)) {
            /* don't drop yet */
            return;
        }
    }

    /* drop data */
    m_data.pop_front();
}

DWORD UncompressedFile::defaultLogContainerSize() const {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_defaultLogContainerSize;
}

void UncompressedFile::setDefaultLogContainerSize(DWORD defaultLogContainerSize) {
    /* mutex lock */
    std::lock_guard<std::mutex> lock(m_mutex);

    /* set default log container size */
    m_defaultLogContainerSize = defaultLogContainerSize;
}

std::shared_ptr<LogContainer> UncompressedFile::logContainerContaining(std::streampos pos) {
    /* loop over all logContainers */
    for (std::shared_ptr<LogContainer> logContainer : m_data) {
        /* when file position is contained ... */
        if ((pos >= logContainer->filePosition) &&
                (pos < logContainer->uncompressedFileSize + logContainer->filePosition)) {

            /* ... return log container */
            return logContainer;
        }
    }

    /* not found, so return nullptr */
    return nullptr;
}

}
}
