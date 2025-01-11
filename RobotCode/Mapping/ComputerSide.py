import math
import car
import graph as g
import time 
import re


# Example of creating a Car object
car = car.Car(0, 0, 0,width=2.0, length=4.5, ip="192.168.137.111")


# Update function for graph

def interactive_update():
    distances_data = []
    distance = 0
    angle = 0
    while True:
        try:
            # Initialize data for both categories
            x_sensed_data, y_sensed_data = [0], [0]  # Starting coordinates for sensed data
            
            car.SendDataESP(f"[{distance},{angle}]") # Send data to ESP32
            
            data = car.getDataESP()
            
            if data is None:
                raise ValueError("Data is None")
            
                
            try:    
                distances_data = [int(x) for x in data[1:-2].split(",")]
            
            except ValueError as e:
                print("Error parsing data:", e)
                distances_data = [0, 0, 0, 0, 0, 0]
                continue
            
            print("Distances_data:", distances_data)
    
            '''
            if distances_data[2] < 400 and distances_data[2] != 0:
                distance = 0
                angle = 15
            
            else:
                distance = 25
                angle = 0
            '''
            distance = 30
            angle = 0
            
            data = car.UseData(distances_data)
            
            
            
            print("Data:", data)
            
            if data is None:
                raise ValueError("Data is None")
            
            else:
                x_sensed_data, y_sensed_data = data
                 
            
            g.updateDataPoint(car.current_x, car.current_y, is_sensed=False)
            g.updateDataList(x_sensed_data, y_sensed_data, is_sensed=True)
            
            g.update_graph()
            
            
            
        except ValueError as e:
            
            if e.args[0] != "Timeout":
                print("Continuing after error: ", e)

            else:
                print("Timed out. Exiting...")
                g.exit()
                break
        

        

def TestConnection():
    while True:
        
        car.SendDataESP("[10,5]")
        
        
        print("Response from ESP32:", car.getDataESP(10))


#car.SendDataESP("[1,1,1,1]") 


interactive_update()

#g.read_map("map.csv")


