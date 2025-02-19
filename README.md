TO DO LIST:
  - clean up the code

CURRENTLY IN PROCESS:
- none

ALREADY IMPLEMENTED:
  - basic functionality
  - bluetooth conrol
  - ADC conntrol
  - PWM-controlled motor speed
  - simple car control function
  - correlation algorithm
  - convolution algorithm
  - function to indicate braking properly
  - if the map has zeros -> it means the end of the map.
  - add function to set true only after a number of positive correlations (#define)
  - add recovery conditions check if correlation is not found after a specific distance
  - command line command to print segments of the map
  - create a MAP of the track and control the car according the map and the real data
  - add ADC values as a safety to override reading-map-changing speed too much
  - add control for LEDs from real-time ADC samples
  - add small braking when approaching a bend section

  FUTURE TASKS:
  - normalize ADC samples to -100; 0; 100
  - test different time intervals for car control and ADC - this will ensure more samples hence better reactions of the car (proven by other students)
  - experiment with different CORR thresholds and windows on the real track

NOTES TO FINDING A NEW LAP START
 - already tried many algorithms
 - the best one so far is Sum of Absolute Differences (SAD).
   - this takes CORRELATION_WINDOW 100 and SLIDING_WINDOW 1, 2, 3 and 5
   - if the SLIDING_WINDOW if 5, misleaded correlation occures from time to time. Values 1, 2 and 3 are very accurate in findind correalation between data amplitudes and shapes representing almost the same function.
   - This function takes SAD_THRESHOLD as a threshold for deciding the correlation. The closer the function to 0 the more the data correlate. Setting the threshold to 1000 experimentally will be tested on a real track.  

