# NoSpark ATMega328P timer usage

## TIMER0
1s'ish general use timer. Used for various timed events.
Accessible through the `system::Timer` class.

## TIMER1
J1772 pilot pulse-width modulation (PWM) timer for J1772 pilot pin.
Used by the `board::J1772Pilot` class.

## TIMER2
Used for AC pin sampling to monitor relay engagement.
Used by the `board::Relays` class.

