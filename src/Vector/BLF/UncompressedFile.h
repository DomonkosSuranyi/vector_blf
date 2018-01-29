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

#include <condition_variable>
#include <mutex>
#include <vector>

#include <Vector/BLF/AbstractFile.h>

#include <Vector/BLF/vector_blf_export.h>

namespace Vector {
namespace BLF {

/**
 * UncompresesdFile (Input/output memory stream)
 *
 * This class is like a virtual file buffer.
 * m_data has a limited view on this virtual file with
 * start position at m_dataBegin and end position dataEnd.
 * In addition read is done at position m_tellg and
 * write position is at m_tellp.
 * Write or seek operations exceeding dataEnd(), extends the view.
 * And explicit dropOldData cuts the begin, so reduced the view.
 *
 * This class is thread-safe.
 */
class VECTOR_BLF_EXPORT UncompressedFile final : public AbstractFile
{
public:
    UncompressedFile();
    ~UncompressedFile();

    virtual void close() override;
    virtual std::streamsize gcount() const override;
    virtual void read(char * s, std::streamsize n) override;
    virtual std::streampos tellg() override;
    virtual void seekg(std::streamoff off, const std::ios_base::seekdir way  = std::ios_base::cur) override;
    virtual void write(const char * s, std::streamsize n) override;
    virtual std::streampos tellp() override;
    virtual void flush() override;
    virtual bool eof() const override;

    /**
     * open, basically resets all variables.
     */
    virtual void open();

    /**
     * Set file size resp. end-of-file position.
     *
     * @param[in] fileSize file size
     */
    virtual void setFileSize(std::streamsize fileSize);

    /**
     * next read will result in eof, as tellp is already >= fileSize.
     */
    virtual bool atEof() const;

    /**
     * Return size of data left to read.
     *
     * @return size
     */
    virtual std::streamsize size() const;

    /**
     * Sets the maximum file size.
     * Write operations block, if the size is reached.
     *
     * @param[in] maxFileSize maximum file size
     */
    virtual void setMaxFileSize(std::streamsize maxFileSize);

    /**
     * drop old data (only if dropSize is possible)
     *
     * @param[in] dropSize size of data to drop (used in write case)
     */
    virtual void dropOldData(std::streamsize dropSize);

    /** tellg was changed (after read or seekg) */
    std::condition_variable tellgChanged;

    /** tellp was changed (after write or seekp) */
    std::condition_variable tellpChanged;

private:
    /** is file open? */
    bool m_is_open;

    /** data */
    std::vector<char> m_data;

    /** data start position */
    std::streampos m_dataBegin;

    /** get position */
    std::streampos m_tellg;

    /** put position */
    std::streampos m_tellp;

    /** last read size */
    std::streamsize m_gcount;

    /** file size */
    std::streamsize m_fileSize;

    /** max file size */
    std::streamsize m_maxFileSize;

    /** error state */
    std::ios_base::iostate m_rdstate;

    /** mutex */
    mutable std::mutex m_mutex;
};

}
}
