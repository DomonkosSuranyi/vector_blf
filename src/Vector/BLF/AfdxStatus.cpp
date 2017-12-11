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

#include <Vector/BLF/AfdxStatus.h>

namespace Vector {
namespace BLF {

AfdxLineStatus::AfdxLineStatus() :
    flags(),
    linkStatus(),
    ethernetPhy(),
    duplex(),
    mdi(),
    connector(),
    clockMode(),
    pairs(),
    reserved(),
    bitrate()
{
}

void AfdxLineStatus::read(AbstractFile & is)
{
    is.read(reinterpret_cast<char *>(&flags), sizeof(flags));
    is.read(reinterpret_cast<char *>(&linkStatus), sizeof(linkStatus));
    is.read(reinterpret_cast<char *>(&ethernetPhy), sizeof(ethernetPhy));
    is.read(reinterpret_cast<char *>(&duplex), sizeof(duplex));
    is.read(reinterpret_cast<char *>(&mdi), sizeof(mdi));
    is.read(reinterpret_cast<char *>(&connector), sizeof(connector));
    is.read(reinterpret_cast<char *>(&clockMode), sizeof(clockMode));
    is.read(reinterpret_cast<char *>(&pairs), sizeof(pairs));
    is.read(reinterpret_cast<char *>(&reserved), sizeof(reserved));
    is.read(reinterpret_cast<char *>(&bitrate), sizeof(bitrate));
}

void AfdxLineStatus::write(AbstractFile & os)
{
    os.write(reinterpret_cast<char *>(&flags), sizeof(flags));
    os.write(reinterpret_cast<char *>(&linkStatus), sizeof(linkStatus));
    os.write(reinterpret_cast<char *>(&ethernetPhy), sizeof(ethernetPhy));
    os.write(reinterpret_cast<char *>(&duplex), sizeof(duplex));
    os.write(reinterpret_cast<char *>(&mdi), sizeof(mdi));
    os.write(reinterpret_cast<char *>(&connector), sizeof(connector));
    os.write(reinterpret_cast<char *>(&clockMode), sizeof(clockMode));
    os.write(reinterpret_cast<char *>(&pairs), sizeof(pairs));
    os.write(reinterpret_cast<char *>(&reserved), sizeof(reserved));
    os.write(reinterpret_cast<char *>(&bitrate), sizeof(bitrate));
}

DWORD AfdxLineStatus::calculateObjectSize() const
{
    return
        sizeof(flags) +
        sizeof(linkStatus) +
        sizeof(ethernetPhy) +
        sizeof(duplex) +
        sizeof(mdi) +
        sizeof(connector) +
        sizeof(clockMode) +
        sizeof(pairs) +
        sizeof(reserved) +
        sizeof(bitrate);
}

AfdxStatus::AfdxStatus() :
    ObjectHeader(),
    channel(),
    statusA(),
    statusB()
{
    objectType = ObjectType::AFDX_STATUS;
}

void AfdxStatus::read(AbstractFile & is)
{
    ObjectHeader::read(is);
    is.read(reinterpret_cast<char *>(&channel), sizeof(channel));
    statusA.read(is);
    statusB.read(is);
}

void AfdxStatus::write(AbstractFile & os)
{
    ObjectHeader::write(os);
    os.write(reinterpret_cast<char *>(&channel), sizeof(channel));
    statusA.write(os);
    statusB.write(os);
}

DWORD AfdxStatus::calculateObjectSize() const
{
    return
        ObjectHeader::calculateObjectSize() +
        sizeof(channel) +
        statusA.calculateObjectSize() +
        statusB.calculateObjectSize();
}

}
}
