import pygtk
pygtk.require('2.0')
import gtk


class MainWindow:
	def delete_event(self, widget, event, data=None):
		self.copter.sendDisconnect()
		return False

	def destroy(self, widget, data=None):
		gtk.main_quit()

	def __init__(self):
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

		self.window.connect("delete_event", self.delete_event)
		self.window.connect("destroy", self.destroy)

		self.window.set_border_width(10)

		self.sensorDataTable = gtk.Table(3,4)
		self.magName = gtk.Label("Magnetometer")
		self.magName.show()
		self.magX = gtk.Label("No Data")
		self.magX.show()
		self.magY = gtk.Label("No Data")
		self.magY.show()
		self.magZ = gtk.Label("No Data")
		self.magZ.show()
		self.accelName = gtk.Label("Accelerometer")
		self.accelName.show()
		self.accelX = gtk.Label("No Data")
		self.accelX.show()
		self.accelY = gtk.Label("No Data")
		self.accelY.show()
		self.accelZ = gtk.Label("No Data")
		self.accelZ.show()
		self.gyroName = gtk.Label("Gyroscope")
		self.gyroName.show()
		self.gyroX = gtk.Label("No Data")
		self.gyroX.show()
		self.gyroY = gtk.Label("No Data")
		self.gyroY.show()
		self.gyroZ = gtk.Label("No Data")
		self.gyroZ.show()

		self.sensorDataTable.attach(self.magName, 0, 1, 0, 1, gtk.SHRINK, gtk.SHRINK, 0, 0)
		self.sensorDataTable.attach(self.magX, 1, 2, 0, 1)
		self.sensorDataTable.attach(self.magY, 2, 3, 0, 1)
		self.sensorDataTable.attach(self.magZ, 3, 4, 0, 1)
		self.sensorDataTable.attach(self.accelName, 0, 1, 1, 2, gtk.SHRINK, gtk.SHRINK, 0, 0)
		self.sensorDataTable.attach(self.accelX, 1, 2, 1, 2)
		self.sensorDataTable.attach(self.accelY, 2, 3, 1, 2)
		self.sensorDataTable.attach(self.accelZ, 3, 4, 1, 2)
		self.sensorDataTable.attach(self.gyroName, 0, 1, 2, 3, gtk.SHRINK, gtk.SHRINK, 0, 0)
		self.sensorDataTable.attach(self.gyroX, 1, 2, 2, 3)
		self.sensorDataTable.attach(self.gyroY, 2, 3, 2, 3)
		self.sensorDataTable.attach(self.gyroZ, 3, 4, 2, 3)

		self.sensorDataTable.show()

		self.window.add(self.sensorDataTable)

		self.tuningTable = gtk.Table(3,2)
		self.gain1Label = gtk.Label("Gain1")
		self.gain1Label.show()
		self.gain2Label = gtk.Label("Gain2")
		self.gain2Label.show()

		self.gain1 = gtk.Entry()
		self.gain1.show()
		self.gain2 = gtk.Entry()
		self.gain2.show()

		self.updateButton = gtk.Button("Update")
		self.updateButton.connect("clicked", self.updateClicked, None)

		self.tuningTable.attach(self.gain1Label,

		self.window.show()

	def main(self):
		gtk.main()

if __name__ == "__main__":
	mainWindow = MainWindow()
	mainWindow.main()
