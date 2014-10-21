
import seabreeze
import pprint
import matplotlib.pyplot as plt
import matplotlib.animation as mplani

pprint.pprint(seabreeze.list_spectrometers())

spec = seabreeze.Spectrometer(index=0)

INTERVAL = 0.3

spec.integration_time(INTERVAL*0.5)

X, Y = spec.spectrum()

fig = plt.figure()
line, = plt.plot(X,Y)

def update_lines(i):
    X, Y = spec.spectrum()
    line.set_data(X,Y)

line_ani = mplani.FuncAnimation(fig, update_lines, interval=INTERVAL, blit=False) 

plt.show()


