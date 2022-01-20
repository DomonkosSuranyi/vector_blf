// SPDX-FileCopyrightText: 2013-2021 Tobias Lorenz <tobias.lorenz@gmx.net>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "platform.h"

#include <atomic>
#include <fstream>
#include <thread>

#include "CompressedFile.h"
#include "FileStatistics.h"
#include "ObjectHeaderBase.h"
#include "ObjectQueue.h"
#include "RestorePoints.h"
#include "UncompressedFile.h"

// UNKNOWN = 0
#include "CanMessage.h" // CAN_MESSAGE = 1
#include "CanErrorFrame.h" // CAN_ERROR = 2
#include "CanOverloadFrame.h" // CAN_OVERLOAD = 3
#include "CanDriverStatistic.h" // CAN_STATISTIC = 4
#include "AppTrigger.h" // APP_TRIGGER = 5
#include "EnvironmentVariable.h" // ENV_INTEGER = 6
#include "EnvironmentVariable.h" // ENV_DOUBLE = 7
#include "EnvironmentVariable.h" // ENV_STRING = 8
#include "EnvironmentVariable.h" // ENV_DATA = 9
#include "LogContainer.h" // LOG_CONTAINER = 10
#include "CanDriverError.h" // CAN_DRIVER_ERROR = 31
#include "CanDriverHwSync.h" // CAN_DRIVER_SYNC = 44
#include "GpsEvent.h" // GPS_EVENT = 46
#include "RealtimeClock.h" // REALTIMECLOCK = 51
// Reserved52 = 52
// Reserved53 = 53
#include "AppText.h" // APP_TEXT = 65
#include "SystemVariable.h" // SYS_VARIABLE = 72
#include "CanErrorFrameExt.h" // CAN_ERROR_EXT = 73
#include "CanDriverErrorExt.h" // CAN_DRIVER_ERROR_EXT = 74
#include "CanMessage2.h" // CAN_MESSAGE2 = 86
#include "SerialEvent.h" // SERIAL_EVENT = 90
#include "DriverOverrun.h" // OVERRUN_ERROR = 91
#include "EventComment.h" // EVENT_COMMENT = 92
#include "GlobalMarker.h" // GLOBAL_MARKER = 96
#include "CanFdMessage.h" // CAN_FD_MESSAGE = 100
#include "CanFdMessage64.h" // CAN_FD_MESSAGE_64 = 101
#include "CanFdErrorFrame64.h"
#include "RestorePointContainer.h" // Unknown115 = 115
// Reserved116 = 116
// Reserved117 = 117
#include "TestStructure.h" // TEST_STRUCTURE = 118
#include "DiagRequestInterpretation.h" // DIAG_REQUEST_INTERPRETATION = 119
#include "FunctionBus.h" // FUNCTION_BUS = 124
#include "DataLostBegin.h" // DATA_LOST_BEGIN = 125
#include "DataLostEnd.h" // DATA_LOST_END = 126
#include "WaterMarkEvent.h" // WATER_MARK_EVENT = 127
#include "TriggerCondition.h" // TRIGGER_CONDITION = 128
#include "CanSettingChanged.h" // CAN_SETTING_CHANGED = 129
#include "DistributedObjectMember.h" // DISTRIBUTED_OBJECT_MEMBER = 130
#include "AttributeEvent.h" // ATTRIBUTE_EVENT = 131

#include "vector_blf_export.h"

namespace Vector {
namespace BLF {

/**
 * File
 *
 * This is similar to std::fstream for BLFs
 */
class VECTOR_BLF_EXPORT File final {
  public:
    File();
    virtual ~File();

    /**
     * File statistics from file header. contains total counts/sizes
     */
    FileStatistics fileStatistics {};

    /**
     * Current uncompressed file size
     *
     * This includes the LogContainer headers, and the uncompressed content.
     */
    uint64_t currentUncompressedFileSize {};

    /**
     * Current number of objects read
     *
     * Unknown115 is not counted.
     */
    std::atomic<uint32_t> currentObjectCount {};

    /**
     * compression level
     *
     * zlib compression levels:
     * - 0: Z_NO_COMPRESSION
     * - 1: Z_BEST_SPEED
     * - 9: Z_BEST_COMPRESSION
     *
     * Vector BLF compression levels:
     * - 0: no compression
     * - 6: default compression
     * - 10: maximum compression
     */
    int compressionLevel {1};

    /**
     * Write restore points at file close.
     */
    bool writeRestorePoints {true};

    /**
     * open file
     *
     * @param[in] filename file name
     * @param[in] mode open mode, either in (read) or out (write)
     */
    virtual void open(const char * filename, const std::ios_base::openmode mode = std::ios_base::in);

    /**
     * open file
     *
     * @param[in] filename file name
     * @param[in] mode open mode, either in (read) or out (write)
     */
    virtual void open(const std::string & filename, const std::ios_base::openmode mode = std::ios_base::in);

    /**
     * is file open?
     *
     * @return true if file is open
     */
    virtual bool is_open() const;

    /**
     * Check whether state of stream is good.
     *
     * @return true if no error flags set
     */
    virtual bool good() const;

    /**
     * Check whether eofbit is set.
     *
     * @return true if end-of-file reached
     */
    virtual bool eof() const;

    /**
     * Read object from file.
     *
     * Ownership is taken over from the library to the user.
     * The user has to take care to delete the object.
     *
     * @todo Use std::unique_ptr in future versions.
     *
     * @return read object or nullptr
     */
    virtual ObjectHeaderBase * read();

    /**
     * Write object to file.
     *
     * Ownership is taken over from the user to the library.
     * The object should not be further accessed any more.
     *
     * @todo Use std::unique_ptr in future versions.
     *
     * @param[in] ohb write object
     */
    virtual void write(ObjectHeaderBase * ohb);

    /**
     * close file
     */
    virtual void close();

    /**
     * Get default log container size.
     *
     * @return default log container size
     */
    virtual uint32_t defaultLogContainerSize() const;

    /**
     * Set default log container size.
     *
     * @param[in] defaultLogContainerSize default log container size
     */
    virtual void setDefaultLogContainerSize(uint32_t defaultLogContainerSize);

    /**
     * create object of given type
     *
     * @param type object type
     * @return new object
     */
    static ObjectHeaderBase * createObject(ObjectType type);

  private:
    /**
     * Open mode
     */
    std::ios_base::openmode m_openMode {};

    /* read/write queue */

    /**
     * read/write queue
     *
     * When the write method is called the object is enqueued here, so that it returns quickly to the calling
     * application. The readWriteThread gets the objects out of the queue and puts them into the compressedFile.
     *
     * When the read method is called the object is taken out of the queue, so that it returns quickly to the calling
     * application. If there are no objects in the queue, the methods waits for the readWriteThread to finish.
     * The readWriteThread reads objects from the compressedfile and puts them into the queue.
     */
    ObjectQueue<ObjectHeaderBase> m_readWriteQueue {};

    /* uncompressed file */

    /**
     * uncompressed file
     *
     * This file contains the data, contained in the (compressed) log containers.
     * The readWriteThread transfers data from/to here into the readWriteQueue.
     * The compressionThread transfers data from/to here into the uncompressedFile.
     */
    UncompressedFile m_uncompressedFile {};

    /**
     * thread between readWriteQueue and uncompressedFile
     */
    std::thread m_uncompressedFileThread {};

    /**
     * exceptions from uncompressedFileThread
     */
    std::exception_ptr m_uncompressedFileThreadException {nullptr};

    /**
     * thread still running
     */
    std::atomic<bool> m_uncompressedFileThreadRunning {};

    /* compressed file */

    /**
     * compressed file
     *
     * This file is actually the fstream, so the actual BLF data.
     * It mainly contains the FileStatistics and several LogContainers carrying the different objects.
     * The compressionThread transfers data from/to here into the compressedFile.
     */
    CompressedFile m_compressedFile {};

    /**
     * thread between uncompressedFile and compressedFile
     */
    std::thread m_compressedFileThread {};

    /**
     * exceptions from compressedFileThread
     */
    std::exception_ptr m_compressedFileThreadException {nullptr};

    /**
     * thread still running
     */
    std::atomic<bool> m_compressedFileThreadRunning {};

    /* internal functions */

    /**
     * Read data from uncompressedFile into readWriteQueue.
     */
    void uncompressedFile2ReadWriteQueue();

    /**
     * Write data from readWriteQueue into uncompressedFile.
     */
    void readWriteQueue2UncompressedFile();

    /**
     * Read/inflate/uncompress data from compressedFile into uncompressedFile.
     */
    void compressedFile2UncompressedFile();

    /**
     * Write/deflate/compress data from uncompressedFile into compressedFile.
     */
    void uncompressedFile2CompressedFile();

    /**
     * transfer data from uncompressedFile to readWriteQueue
     */
    static void uncompressedFileReadThread(File * file);

    /**
     * transfer data from readWriteQueue to uncompressedfile
     */
    static void uncompressedFileWriteThread(File * file);

    /**
     * transfer data from compressedFile to uncompressedFile
     */
    static void compressedFileReadThread(File * file);

    /**
     * transfer data from uncompressedfile to compressedFile
     */
    static void compressedFileWriteThread(File * file);
};

}
}
