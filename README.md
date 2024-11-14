TO DO LIST:
  - frame ADC values (1950 up to 1980, etc, maybe even right in the filter)
  - clean up the code

CURRENTLY IN PROGRESS:
  - fiding a new lap start
  - FSM for car control

ALREADY IMPLEMENTED:
  - basic functionality
  - bluetooth conrol
  - ADC conntrol
  - PWM-controlled motor speed
  - simple car control function
  - correlation algorithm
  - convolution algorithm

NOTES TO FINDING A NEW LAP START
 - already tried many algorithms
 - the best one so far is Sum of Absolute Differences (SAD).
   - this takes CORRELATION_WINDOW 100 and SLIDING_WINDOW 1, 2, 3 and 5
   - if the SLIDING_WINDOW if 5, misleaded correlation occures from time to time. Values 1, 2 and 3 are very accurate in findind correalation between data amplitudes and shapes representing almost the same function.
   - This function takes SAD_THRESHOLD as a threshold for deciding the correlation. The closer the function to 0 the more the data correlate. Setting the threshold to 1000 experimentally will be tested on a real track.  

