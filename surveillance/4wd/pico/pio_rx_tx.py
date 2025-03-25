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

from rp2 import PIO, StateMachine, asm_pio
import rp2
from machine import Pin

# Example using PIO to create a UART TX interface


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


@asm_pio(sideset_init=PIO.OUT_HIGH, out_init=PIO.OUT_HIGH, out_shiftdir=PIO.SHIFT_RIGHT)
def uart_tx():
    # Block with TX deasserted until data available
    pull()
    # Initialise bit counter, assert start bit for 8 cycles
    set(x, 7).side(0)[7]
    # Shift out 8 data bits, 8 execution cycles per bit
    label("bitloop")
    out(pins, 1)[6]
    jmp(x_dec, "bitloop")
    # Assert stop bit for 8 cycles total (incl 1 for pull())
    nop().side(1)[6]


class SerialRxTx:
    def __init__(self, rx, tx, boudrate=115200):
        self.pin_rx = Pin(rx, Pin.IN, Pin.PULL_UP)
        self.pin_tx = Pin(tx, Pin.OUT)
        self.boudrate = boudrate

        self.sm_tx = StateMachine(
            0,
            globals()["uart_tx"],
            freq=8 * self.boudrate,
            sideset_base=self.pin_tx,
            out_base=self.pin_tx,
        )
        self.sm_tx.active(1)
        # Set up the state machine we're going to use to receive the characters.
        self.sm_rx = StateMachine(
            1,
            globals()["uart_rx"],
            freq=8 * self.boudrate,
            in_base=self.pin_rx,  # For WAIT, IN
            jmp_pin=self.pin_rx,  # For JMP
        )
        self.sm_rx.active(1)

    def put(self,value):
        for s in value:
            self.sm_tx.put(s)

    def readline(self, buffer=1024):
        val = chr(self.sm_rx.get() >> 24)
        retval = val
        while val != "\n":
            val = chr(self.sm_rx.get() >> 24)
            if val == "\r" or val == "\n":
                pass
            else:
                retval += val
        return retval
