#brailleDisplayDrivers/scribe.py
#A part of NonVisual Desktop Access (NVDA)
#This file is covered by the GNU General Public License.
#See the file COPYING for more details.
#Copyright (C) 2012-2017 Tobias Platen, Halim Sahin, Ali-Riza Ciftcioglu, NV Access Limited, Davy Kager
#Author: Tobias Platen (nvda@lists.thm.de)
#minor changes by Halim Sahin (nvda@lists.thm.de), Ali-Riza Ciftcioglu <aliminator83@googlemail.com>, James Teh and Davy Kager

import braille
from typing import List
import sys
import serial
import hwPortUtils
from logHandler import log

class BrailleDisplayDriver(braille.BrailleDisplayDriver):
	"""scribe braille display driver."""
	name = "Scribe"
	description = "Scribe braille display"
	numCells = 1

	@classmethod
	def check(cls):
		'''determines if this driver is available'''
		return True
	
	def connectBluetooth(self, esp_mac_addr: str):
		for portInfo in sorted(hwPortUtils.listComPorts(onlyAvailable=True), key=lambda item: "bluetoothName" in item):
			port = portInfo["port"]
			if "bluetoothName" in portInfo:
				if portInfo["bluetoothName"][0:5] == "Scribe":
					try:
						self._ser = serial.Serial(port, baudrate = 115200,timeout = .2, writeTimeout = .2)
						log.info("connectBluetooth success")
					except:
						log.debugWarning("connectBluetooth failed")
		
	def terminate(self):
		try:
			super(BrailleDisplayDriver, self).terminate()
		finally:
			self._ser.close()

	def __init__(self):
		"""initialize driver"""
		super(BrailleDisplayDriver, self).__init__()
		if self.scribe is None:
			self.connectBluetooth(self.addr)

	def display(self, cells: List[int]):
		"""write to braille display"""
		self._ser.write(cells)