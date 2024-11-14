# please install matplotlib before running this script
# pip install pyserial matplotlib

# tested with python 3.12.3 
# output .csv file will be stored in the same directory as the script

import serial
import matplotlib.pyplot as plt
import time
import csv
from datetime import datetime

# set serial port and baudrate
port = 'COM3'
baudrate = 115200

# open serial port
ser = serial.Serial(port, baudrate)

timestamps = []
data = []
max_points = 200  # max number of points in plot

# CSV file to store data
csv_filename = f"data_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
with open(csv_filename, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["Timestamp", "Value"])

    # start plotting received data
    plt.ion()
    fig, ax = plt.subplots()
    line, = ax.plot(timestamps, data)

    try:
        while True:
            if ser.in_waiting > 0:
                line_data = ser.readline().decode('utf-8').strip()
                
                try:
                    # new data point
                    value = float(line_data)
                    current_time = time.time()
                    formatted_time = datetime.fromtimestamp(current_time).strftime('%d.%m.%Y %H:%M')

                    # add new point to corresponding lists
                    timestamps.append(current_time)
                    data.append(value)
                    
                    # write to CSV file
                    writer.writerow([formatted_time, value])
                    file.flush()  # pridané pre istotu, aby sa údaj zapísal... nevyriešilo to ale divné obrazovanie v exceli

                    # newst 200 values are plotted
                    if len(timestamps) > max_points:
                        timestamps = timestamps[-max_points:]
                        data = data[-max_points:]
                    
                    # new data point added to graph
                    line.set_xdata(timestamps)
                    line.set_ydata(data)
                    
                    ax.set_xticks(timestamps[::len(timestamps)//10+1])  # every 10th timestamp is shown (s vyššou hodnotou okno celkom po n hodnotách padne/vyvolá sa ValueError nižšie - treba doriešiť)
                    ax.set_xticklabels([datetime.fromtimestamp(t).strftime('%H:%M:%S') for t in timestamps[::len(timestamps)//10+1]])
                    
                    # axis scaling
                    ax.relim()
                    ax.autoscale_view()
                    
                    plt.draw()
                    plt.pause(0.01)  # set time to refresh plot

                except ValueError:
                    print(f"Unsupported data value format: {line_data}")

    except KeyboardInterrupt:
        print("Script terminated by user.")
    finally:
        ser.close()
        plt.ioff()
        plt.show() 
