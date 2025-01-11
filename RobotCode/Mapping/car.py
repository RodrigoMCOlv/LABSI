import math
import requests
import time

class Car:
    def __init__(self, x , y , angle , width , length, ip = "192.168.137.38"):
        # Initialize the attributes
        self.current_angle = angle
        self.current_x = x 
        self.current_y = y
        self.width = width
        self.length = length
        
        self.esp_ip = ip
        self.post_url = f"http://{self.esp_ip}/post"
        self.get_url = f"http://{self.esp_ip}/getData"
        
    
    def UseData(self, distances):
        
        try:
            x_info = []
            y_info = []
            
            #Will get a string "[distance moved, ang, 1,2,3,4]" and we want to parse to its individual elements

            
            print("Distances:", distances)
            
                        
            self.current_angle += distances[1]
            
            self.current_x = round( self.current_x + math.cos(math.radians(self.current_angle))*distances[0] , 2)
            self.current_y = round( self.current_y + math.sin(math.radians(self.current_angle))*distances[0] , 2)
            
            print(f"X: {self.current_x} Y: {self.current_y} Angle: {self.current_angle}")
            
            distances.pop(0)
            distances.pop(0)
            
            print("Distances:", distances)
            
            
            x_info, y_info = self.Calculate_points(distances)
            
            return x_info, y_info
        
        except requests.exceptions.RequestException as e:
            print("Error fetching data from ESP32:", e)
            return None   
          
    def Calculate_points(self, dist):
        ang = 0
        x_data = []
        y_data = []
        
        
        
        if dist[0] == 0:
            x = 0
            y = 0
        
        else:
            x = round( math.cos(math.radians(ang+self.current_angle))*dist[0] + self.current_x , 2)
            y = round( math.sin(math.radians(ang+self.current_angle))*dist[0] + self.current_y , 2)
            
            print(f"X: {x} Y: {y}")

            x_data.append(x)
            y_data.append(y)
        
        ang -= 90
        
        
        if dist[1] == 0:
            x = 0
            y = 0
        
        else:
            x = round( math.cos(math.radians(ang+self.current_angle))*dist[1] + self.current_x , 2)
            y = round( math.sin(math.radians(ang+self.current_angle))*dist[1] + self.current_y , 2)
            
            print(f"X: {x} Y: {y}")

            x_data.append(x)
            y_data.append(y)
            
        ang -= 90
        
        if dist[2] == 0:    
            x = 0
            y = 0
        
        else:
            x = round( math.cos(math.radians(ang+self.current_angle))*dist[2] + self.current_x , 2)
            y = round( math.sin(math.radians(ang+self.current_angle))*dist[2] + self.current_y , 2)
            
            print(f"X: {x} Y: {y}")

            x_data.append(x)
            y_data.append(y)
            
        ang -= 90
        
        if dist[3] == 0:
            x = 0
            y = 0        
        
        else:        
            x = round( math.cos(math.radians(ang+self.current_angle))*dist[3] + self.current_x , 2)
            y = round( math.sin(math.radians(ang+self.current_angle))*dist[3] - self.current_y , 2)
            
            print(f"X: {x} Y: {y}")

            x_data.append(x)
            y_data.append(y)
            
        
        
        '''
        for d in dist:   
            if d == 0:
                x = 0
                y = 0
                
            else:
                x = round( self.current_x + math.cos(math.radians(ang+self.current_angle))*d , 2)
                y = round( self.current_y + math.sin(math.radians(ang+self.current_angle))*d , 2)
            
                print(f"X: {x} Y: {y}")
            
                x_data.append(x)
                y_data.append(y)
            
            ang -= 90
        
        '''
        
        print(f"Returning X: {x_data} Y: {y_data}")
        return x_data, y_data 
            
    def SendDataESP(self, data):
        requests.post(self.post_url, data=data)
        print("Data sent to ESP32")
               
    def getDataESP(self, retries = 15  ,timeout = 1):
        tries = 0
        
        while tries < retries:  # Keep trying until valid data is received
            print("Waiting for ESP32 to respond...")
            tries +=1
            
            try:
                response = requests.get(self.get_url, timeout=timeout)
                
                # Check if the response is valid (e.g., non-empty and properly formatted)
                if response.status_code == 200 and response.text.strip():
                    
                    if response.text == "No data available":
                        print("ESP32 had no data available")
                        
                        time.sleep(0.5)
                        
                        continue    # Retry fetching data
                    else:
                        return response.text  # Return the valid response
                
                print("Invalid response received, retrying...")
            
            except requests.exceptions.RequestException as e:
                print("Error fetching data from ESP32:", e)
            
        
        return "Timeout"  # Return a message indicating a timeout
            
        
        
    