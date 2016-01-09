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

#include "LinDlcInfo.h"

namespace Vector {
namespace BLF {

LinDlcInfo::LinDlcInfo() :
    ObjectHeader(),
    channel(),
    id(),
    dlc(),
    reserved()
{
    objectType = ObjectType::LIN_DLC_INFO;
}

void LinDlcInfo::read(std::istream & is)
{
    ObjectHeader::read(is);
    is.read((char *) &channel, sizeof(channel));
    is.read((char *) &id, sizeof(id));
    is.read((char *) &dlc, sizeof(dlc));
    is.read((char *) reserved.data(), reserved.size());
}

void LinDlcInfo::write(std::ostream & os)
{
    ObjectHeader::write(os);
    os.write((char *) &channel, sizeof(channel));
    os.write((char *) &id, sizeof(id));
    os.write((char *) &dlc, sizeof(dlc));
    os.write((char *) reserved.data(), reserved.size());
}

size_t LinDlcInfo::calculateObjectSize()
{
    size_t size =
        ObjectHeader::calculateObjectSize() +
        sizeof(channel) +
        sizeof(id) +
        sizeof(dlc) +
        reserved.size();

    return size;
}

}
}
