
import time
import configuration
import _thread

UART_BAUD = 9600



def _get_distance():
    global _current_distance
    distance_now = configuration.MAX_RANGE_SENSOR
    H_DATA = 0
    L_DATA = 0
    SUM = 0
    configuration.SR05EN.low()
    #S_DATA = _sm.get() >> 24
    #if S_DATA == 0xFF:
    #    H_DATA = _sm.get() >> 24
    #    L_DATA = _sm.get() >> 24
    #    SUM = _sm.get() >> 24
    configuration.SR05EN.high()
    #if S_DATA == 0xFF and H_DATA == 0xAA and L_DATA == 0xAA and SUM == 0x53:
    distance_now = configuration.MAX_RANGE_SENSOR
    #else:
    #    distance = H_DATA * 256 + L_DATA
    #    sum = S_DATA + H_DATA + L_DATA
    #    sum = sum & 0xFF
    #    if SUM == sum:
    #        distance_now = distance / 10
    _lock.acquire()
    _current_distance = distance_now
    _lock.release()


_lock = _thread.allocate_lock()


def get_distance():
    global _current_distance
    global _lock
    _lock.acquire()
    distance = _current_distance
    _lock.release()
    return distance


def _readcontinous():
    global isRunning
    isRunning = True
    if configuration.IOE_SR05_DISABLE:
        isRunning = False
        global _current_distance
        _current_distance = configuration.STOP_DISTANCE + 100
    while isRunning:
        _get_distance()
        time.sleep_ms(20)
    _thread.exit()


if configuration.IOE_SR05_DISABLE:
    global isRunning
    global _current_distance
    isRunning = False
    _current_distance = configuration.STOP_DISTANCE + 100
else:
    global isRunning
    global _current_distance
    isRunning = True
    _current_distance = 0
    _thread.start_new_thread(_readcontinous, ())


def stop_second_core():
    global isRunning
    isRunning = False
