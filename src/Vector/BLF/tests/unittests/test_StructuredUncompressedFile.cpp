#define BOOST_TEST_MODULE StructuredUncompressedFile
#if !defined(WIN32)
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <Vector/BLF.h>

/** Open a file, read/write on it, close it again. */
BOOST_AUTO_TEST_CASE(ReadTest) {
    Vector::BLF::RawCompressedFile rawCompressedFile;
    Vector::BLF::StructuredCompressedFile structuredCompressedFile(rawCompressedFile);
    Vector::BLF::RawUncompressedFile rawUncompressedFile(structuredCompressedFile);
    Vector::BLF::StructuredUncompressedFile structuredUncompressedFile(rawUncompressedFile);

    /* checks after initialize */
    BOOST_CHECK(!structuredUncompressedFile.is_open());
    BOOST_CHECK_EQUAL(structuredUncompressedFile.tellg(), 0); // @todo should be -1
    BOOST_CHECK_EQUAL(structuredUncompressedFile.tellp(), 0); // @todo should be -1

    /* open file */
    structuredUncompressedFile.open(CMAKE_CURRENT_SOURCE_DIR "/events_from_binlog/test_CanMessage.blf", std::ios_base::in);
    BOOST_CHECK(structuredUncompressedFile.is_open());
    BOOST_CHECK_EQUAL(structuredUncompressedFile.tellg(), 0);
    BOOST_CHECK_EQUAL(structuredUncompressedFile.tellp(), 0);

    /* read object 0/1 */
    Vector::BLF::ObjectHeaderBase * ohb = structuredUncompressedFile.read();
    BOOST_CHECK(ohb);
    BOOST_CHECK(structuredUncompressedFile.is_open());
    BOOST_CHECK_EQUAL(structuredUncompressedFile.tellg(), 1);
    BOOST_CHECK_EQUAL(structuredUncompressedFile.tellp(), 0);
    delete ohb;

    /* read object 1/1 */
    ohb = structuredUncompressedFile.read();
    BOOST_CHECK(ohb);
    BOOST_CHECK(structuredUncompressedFile.is_open());
    BOOST_CHECK_EQUAL(structuredUncompressedFile.tellg(), 2);
    BOOST_CHECK_EQUAL(structuredUncompressedFile.tellp(), 0);
    delete ohb;

    /* rewind to beginning of file */
    structuredUncompressedFile.seekg(0, std::ios_base::beg);
    BOOST_CHECK_EQUAL(structuredUncompressedFile.tellg(), 0);

    /* close file */
    structuredUncompressedFile.close();
    BOOST_CHECK(!structuredUncompressedFile.is_open());
}
