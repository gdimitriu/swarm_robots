# the pio rx is from
# https://github.com/micropython/micropython/blob/master/examples/rp2/pio_uart_rx.py
#
# Example using PIO to create a UART RX interface.
#
# To make it work you'll need a wire connecting GPIO4 and GPIO3.
#
# Demonstrates:
#   - PIO shifting in data on a pin
#   - PIO jmp(pin) instruction
#   - PIO irq handler


# ruff: noqa: F821 - @asm_pio decorator adds names to function scope

from rp2 import StateMachine, asm_pio
import rp2
import time
import configuration
import _thread

UART_BAUD = 9600


@asm_pio(
    in_shiftdir=rp2.PIO.SHIFT_RIGHT,
)
def uart_rx():
    # fmt: off
    label("start")
    # Stall until start bit is asserted
    wait(0, pin, 0)
    # Preload bit counter, then delay until halfway through
    # the first data bit (12 cycles incl wait, set).
    set(x, 7)[10]
    label("bitloop")
    # Shift data bit into ISR
    in_(pins, 1)
    # Loop 8 times, each loop iteration is 8 cycles
    jmp(x_dec, "bitloop")[6]
    # Check stop bit (should be high)
    jmp(pin, "good_stop")
    # Either a framing error or a break. Set a sticky flag
    # and wait for line to return to idle state.
    irq(block, 4)
    wait(1, pin, 0)
    # Don't push data if we didn't see good framing.
    jmp("start")
    # No delay before returning to start; a little slack is
    # important in case the TX clock is slightly too fast.
    label("good_stop")
    push(block)
    # fmt: on


# The handler for a UART break detected by the PIO.
# def handler(sm):
#    print("break", time.ticks_ms(), end=" ")


# Set up the state machine we're going to use to receive the characters.
_sm = StateMachine(
    0,
    globals()["uart_rx"],
    freq=8 * UART_BAUD,
    in_base=configuration.PIO_RX_PIN,  # For WAIT, IN
    jmp_pin=configuration.PIO_RX_PIN,  # For JMP
)


# sm.irq(handler)


def _get_distance():
    global _current_distance
    global _sm
    distance_now = configuration.MAX_RANGE_SENSOR
    H_DATA = 0
    L_DATA = 0
    SUM = 0
    configuration.SR05EN.low()
    S_DATA = _sm.get() >> 24
    if S_DATA == 0xFF:
        H_DATA = _sm.get() >> 24
        L_DATA = _sm.get() >> 24
        SUM = _sm.get() >> 24
    configuration.SR05EN.high()
    if S_DATA == 0xFF and H_DATA == 0xAA and L_DATA == 0xAA and SUM == 0x53:
        distance_now = configuration.MAX_RANGE_SENSOR
    else:
        distance = H_DATA * 256 + L_DATA
        sum = S_DATA + H_DATA + L_DATA
        sum = sum & 0xFF
        if SUM == sum:
            distance_now = distance / 10
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
    _sm.active(1)
    isRunning = True
    _current_distance = 0
    _thread.start_new_thread(_readcontinous, ())


def stop_second_core():
    global isRunning
    isRunning = False
