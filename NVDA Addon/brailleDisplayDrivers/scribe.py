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
	numCells = 100
	mapString = " A1B'K2L@CIF/MSP\"E3H9O6R^DJG>NTQ,*5<-U8V.%[$+X!&;:4\\0Z7(_?W]#Y)="
	_ser = None
	@classmethod
	def check(cls):
		'''determines if this driver is available'''
		return True
	
	def connectBluetooth(self):
		for portInfo in sorted(hwPortUtils.listComPorts(onlyAvailable=True), key=lambda item: "bluetoothName" in item):
			port = portInfo["port"]
			#print portInfo
			if "bluetoothName" in portInfo:
				log.info("connectBluetooth %s"%portInfo["bluetoothName"])
				if "Scribe" in portInfo["bluetoothName"]:
					log.info("attempting to connect to scribe")
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
		log.info("init")
		super(BrailleDisplayDriver, self).__init__()
		if self._ser is None:
			self.connectBluetooth()

	def display(self, cells: List[int]):
		"""write to braille display"""
		for i in range(len(cells)):
			if cells[i] > 64:
				log.info(cells[i])
				cells[i] = 0
		newString += " " + "".join([self.mapString[i] for i in cells])
		#remove trailing spaces
		newString = newString.rstrip()
		log.info(newString)
		#convert to utf-8
		newString = newString.encode("utf-8")
		self._ser.write(newString)