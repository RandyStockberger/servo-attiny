# servo-attiny

Software for the ATTiny4313 to control two model railroad turnouts via servo

Use PWM from the 16 bit timer 1 to generate the signal to drive a pair of RC
servos. The servos are used to position the switch points of a pair of model
railroad turnouts.

Additional port pins are used to drive LEDs indicating the current position
of each turnout and to monitor the state of toggle switches used to indicate
the turnouts should change state.

Additional Features:
	- The turnout points take approx. two seconds to move from one state to
	  the other. This reduces wear and stress on the turnout.
	- The final resting positions of the servo, and therefore the turnout
	  points is tuneable, again to reduce wear and stress on the turnout.
	- A SPDT relay is used to switch power to the turnout frog. Powering the
	  frog with the correct polarity of DC or DCC signal prevents a short
	  circuit or stall when the train is located on the frog.
