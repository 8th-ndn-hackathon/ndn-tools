/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016-2019, Regents of the University of California,
 *                          Colorado State University,
 *                          University Pierre & Marie Curie, Sorbonne University.
 *
 * This file is part of ndn-tools (Named Data Networking Essential Tools).
 * See AUTHORS.md for complete list of ndn-tools authors and contributors.
 *
 * ndn-tools is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndn-tools is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndn-tools, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * @author Chavoosh Ghasemi <chghasemi@cs.arizona.edu>
 */

#include "discover-version-realtime.hpp"

#include <ndn-cxx/metadata-object.hpp>

namespace ndn {
namespace chunks {

DiscoverVersionRealtime::DiscoverVersionRealtime(const Name& prefix,
                                                 Face& face,
                                                 const Options& options)
  : chunks::Options(options)
  , DiscoverVersion(prefix, face)
  , Options(options)
{
}

void
DiscoverVersionRealtime::run()
{
  expressInterest(MetadataObject::makeDiscoveryInterest(m_prefix)
                  .setInterestLifetime(discoveryTimeout),
                  maxRetriesOnTimeoutOrNack, maxRetriesOnTimeoutOrNack);
}

void
DiscoverVersionRealtime::handleData(const Interest& interest, const Data& data)
{
  if (isVerbose)
    std::cerr << "Data: " << data << std::endl;

  // make a metadata object from received metadata packet
  MetadataObject mobject;
  try {
    mobject = MetadataObject(data);
  }
  catch (const tlv::Error& e) {
    this->emitSignal(onDiscoveryFailure, "Invalid metadata packet: "s + e.what());
    return;
  }

  if (mobject.getVersionedName().empty() || !mobject.getVersionedName()[-1].isVersion()) {
    this->emitSignal(onDiscoveryFailure, mobject.getVersionedName().toUri() +
                                         " is not a valid versioned name");
    return;
  }

  if (isVerbose) {
    std::cerr << "Discovered Data version: " << mobject.getVersionedName()[-1].toVersion() << std::endl;
  }
  this->emitSignal(onDiscoverySuccess, mobject.getVersionedName());
}

} // namespace chunks
} // namespace ndn
