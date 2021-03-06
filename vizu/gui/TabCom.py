#!/usr/bin/python3
# -*- coding: utf-8 -*-

from PyQt5.Qt import *
from PyQt5.QtWidgets import *

class TabCom(QWidget):
    
    #QT emitted signals :
    networkStatus = pyqtSignal(bool)
    
    #@param ArdSerial : a reference on the object managing the serial line
    def __init__(self, comMdw):
        super().__init__()
        self.com = comMdw
        ports, baudrates = self.com.getSerialPortInfo()
        
        settings = QSettings("config.ini", QSettings.IniFormat)
        settings.beginGroup("Com")
        defaultPort = settings.value("port", "COM1")
        defaultBaudrate = settings.value("baudrate", "250000")
        
        #Serial port configuration
            #COM port selector
        self.combo_COM = QComboBox(self)
        self.combo_COM.addItems(ports)
        i = self.combo_COM.findText(defaultPort)
        if i != -1 :
            self.combo_COM.setCurrentIndex(i)
            #Baudrate selector
        self.combo_Baudrate = QComboBox(self)
        for baudrate in baudrates:
            self.combo_Baudrate.addItem(str(baudrate), baudrate)
        i = self.combo_Baudrate.findData(defaultBaudrate)
        if i != -1 :
            self.combo_Baudrate.setCurrentIndex(i)
            #connection button
        self.btn_connect = QPushButton('Connect', self)
        self.btn_connect.setCheckable(True)
        self.btn_connect.toggled[bool].connect(self._connectFromButton)
        
        layout = QVBoxLayout(self)
        layoutH1 = QHBoxLayout()
        layout.addLayout(layoutH1)
        layout.addStretch()
        
        layoutH1.addWidget(self.combo_COM)
        layoutH1.addWidget(self.combo_Baudrate)
        layoutH1.addWidget(self.btn_connect)      
        layoutH1.addStretch()
        
        #keyboard shortcuts
        QShortcut(QKeySequence(Qt.Key_C), self).activated.connect(self._connectFromShorcut)

    @pyqtSlot(bool)
    def _connectFromButton(self, pressed):
        if pressed:
            self._connect()
        else:
            self._disconnect()
        
    def _connectFromShorcut(self):
        self.btn_connect.toggle()            
        
    def _connect(self):
        port = self.combo_COM.currentText()
        baudrate = self.combo_Baudrate.currentData()
        print("Connection request on port " + port + " at a baudrate of " + str(baudrate))
        self.combo_COM.setEnabled(False)
        self.combo_Baudrate.setEnabled(False)
        self.btn_connect.setText("Disconnect")
        
        if self.com.connect(port, baudrate):
            print("Connected")
            settings = QSettings("config.ini", QSettings.IniFormat)
            settings.beginGroup("Com")
            settings.setValue("port", port)
            settings.setValue("baudrate", baudrate)
            self.networkStatus.emit(True)
        else:
            print("ERROR : Connection failed, check that the device is connected to the right port, and that nothing is holding the COM PORT (like another vizy instance...)")
            self.btn_connect.setText("Connect")
            self.btn_connect.setChecked(False)
            self.combo_COM.setEnabled(True)
            self.combo_Baudrate.setEnabled(True)
            
    def _disconnect(self):
        self.com.disconnect()
        self.btn_connect.setText("Connect")
        self.btn_connect.setChecked(False)
        self.combo_COM.setEnabled(True)
        self.combo_Baudrate.setEnabled(True)
        self.networkStatus.emit(False)
        print("Disconnected")
            
if __name__ == '__main__':
    import sys
    app = QApplication(sys.argv)
    widget = TabCom()
    widget.show()
    sys.exit(app.exec_())