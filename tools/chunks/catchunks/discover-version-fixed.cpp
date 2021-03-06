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
 * @author Andrea Tosatto
 */

#include "discover-version-fixed.hpp"

namespace ndn {
namespace chunks {

DiscoverVersionFixed::DiscoverVersionFixed(const Name& prefix, Face& face, const Options& options)
  : Options(options)
  , DiscoverVersion(prefix, face)
{
}

void
DiscoverVersionFixed::run()
{
  Interest interest(m_prefix);
  interest.setInterestLifetime(interestLifetime);
  interest.setMustBeFresh(mustBeFresh);
  interest.setMaxSuffixComponents(2);
  interest.setMinSuffixComponents(2);

  expressInterest(interest, maxRetriesOnTimeoutOrNack, maxRetriesOnTimeoutOrNack);
}

void
DiscoverVersionFixed::handleData(const Interest& interest, const Data& data)
{
  if (isVerbose)
    std::cerr << "Data: " << data << std::endl;

  // data name must end with a segment number, preceded by a version number
  if (data.getName().size() >= 2 &&
      data.getName()[-1].isSegment() &&
      data.getName()[-2].isVersion()) {
    if (isVerbose)
      std::cerr << "Found data with the requested version: " << data.getName()[-2] << std::endl;

    this->emitSignal(onDiscoverySuccess, data.getName().getPrefix(-1));
  }
  else {
    // data isn't a valid segment, add to the exclude list
    m_strayExcludes.excludeOne(data.getName()[-1]);
    Interest newInterest(interest);
    newInterest.refreshNonce();
    newInterest.setExclude(m_strayExcludes);

    expressInterest(newInterest, maxRetriesOnTimeoutOrNack, maxRetriesOnTimeoutOrNack);
  }
}

} // namespace chunks
} // namespace ndn
