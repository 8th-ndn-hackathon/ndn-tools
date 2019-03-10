# -*- Mode:python; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
#
# Copyright (C) 2015-2018, The University of Memphis,
#                          Arizona Board of Regents,
#                          Regents of the University of California.
#
# This file is part of Mini-NDN.
# See AUTHORS.md for a complete list of Mini-NDN authors and contributors.
#
# Mini-NDN is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Mini-NDN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Mini-NDN, e.g., in COPYING.md file.
# If not, see <http://www.gnu.org/licenses/>.

from ndn.experiments.experiment import Experiment

import time

class ChunksExperiment(Experiment):

    def __init__(self, args):

        Experiment.__init__(self, args)

    def setup(self):
        #if str(self.options.arguments.chunksType) == "ndn":
        print(self.net['host'].cmd("nfdc face create udp://1.0.0.2"))
        print(self.net['router'].cmd("nfdc face create udp://1.0.0.6"))
        print(self.net['host'].cmd("nfdc route add / udp://1.0.0.2"))
        print(self.net['router'].cmd("nfdc route add / udp://1.0.0.6"))
        #self.net['server'].cmd("nlsrc advertise /50m")
        print(self.net['server'].cmd("ndnputchunks /50m < /vagrant/mini-ndn/50m.dat &> /dev/null &"))
        time.sleep(30)
        print(self.net['host'].cmd("ndncatchunks --use-cubic --ignore-cong-marks /50m &>> chunks-output"))
        self.net['host'].cmd("echo '----------------------------------\n' >> chunks-output")
	time.sleep(60)
        #else:
        self.net['host'].cmd("echo '----------------ip----------------\n' >> chunks-output")
        self.net['host'].cmd("ip route add 1.0.0.6 via 1.0.0.2")
        self.net['server'].cmd("ip route add 1.0.0.1 via 1.0.0.5")
        self.net['server'].cmd("iperf3 -s &> /dev/null &")
        self.net['host'].cmd("iperf3 --bytes  50M -c 1.0.0.6 >> chunks-output")
        self.net['host'].cmd("echo '----------------------------------\n' >> chunks-output")
        self.net['host'].cmd("ping 1.0.0.6 -c 10 >> chunks-output")
        self.net['host'].cmd("echo '----------------------------------\n' >> chunks-output")


    def run(self):
        pass

    @staticmethod
    def parseArguments(parser):
        parser.add_argument("--chunksType", dest="chunksType", default="ndn",
                            help="sdgerg")

Experiment.register("chunks", ChunksExperiment)
