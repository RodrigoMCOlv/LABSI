import matplotlib.pyplot as plt
import csv

# Initialize data for both categories
x_car_data, y_car_data = [0], [0]  # Starting coordinates for car
x_sensed_data, y_sensed_data = [0], [0]  # Starting coordinates for sensed data

# Create figure and axis
fig, ax = plt.subplots()

# Plot car coordinates (blue points)
graph_car = ax.scatter(x_car_data, y_car_data, color="blue", label="Car Path")

# Plot sensed data (red points)
graph_sensed = ax.scatter(x_sensed_data, y_sensed_data, color="red", label="Sensed Data")

ax.set_xlim(-100, 100)
ax.set_ylim(-100, 100)
ax.axhline(0, color="black", linewidth=0.5)
ax.axvline(0, color="black", linewidth=0.5)
ax.set_title("Dynamic Coordinate Plot")
ax.legend()

plt.ion()  # Turn on interactive mode
plt.show()

def cleanData(x, y):
    """
    Removes (0, 0) pairs from the provided lists x and y.
    Returns the cleaned lists.
    """
    cleaned_x = []
    cleaned_y = []
    
    for x_coord, y_coord in zip(x, y):
        if (x_coord, y_coord) != (0, 0):  # Skip (0, 0) pairs
            cleaned_x.append(x_coord)
            cleaned_y.append(y_coord)
    
    return cleaned_x, cleaned_y

def update_graph():
    global x_car_data, y_car_data, x_sensed_data, y_sensed_data
    
    # Update car coordinates (blue points)
    graph_car.set_offsets(list(zip(x_car_data, y_car_data)))
    
    # Update sensed data (red points)
    graph_sensed.set_offsets(list(zip(x_sensed_data, y_sensed_data)))
    
    x_sensed_data, y_sensed_data = cleanData(x_sensed_data, y_sensed_data)
    
    # Adjust axis limits dynamically based on both sets of data
    ax.set_xlim(min(x_car_data + x_sensed_data) - 1, max(x_car_data + x_sensed_data) + 1)
    ax.set_ylim(min(y_car_data + y_sensed_data) - 1, max(y_car_data + y_sensed_data) + 1)
    
    # Redraw the graph
    fig.canvas.draw()
    fig.canvas.flush_events()
     
def updateDataPoint(x, y, is_sensed=False):
    global x_car_data, y_car_data, x_sensed_data, y_sensed_data
    
    if is_sensed:
        x_sensed_data.append(x)
        y_sensed_data.append(y)
    else:
        x_car_data.append(x)
        y_car_data.append(y)
    
    update_graph()  # Trigger graph update

def updateDataList(x, y, is_sensed=False):
    global x_car_data, y_car_data, x_sensed_data, y_sensed_data
    
    if is_sensed:
        x_sensed_data.extend(x)
        y_sensed_data.extend(y)
    else:
        x_car_data.extend(x)
        y_car_data.extend(y)
    
    update_graph()  # Trigger graph update

def exit():
    # Save both categories (car and sensed) to "map.csv"
    with open("map.csv", mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["Car_X", "Car_Y", "Sensed_X", "Sensed_Y"])  # Header row
        for car_x, car_y, sensed_x, sensed_y in zip(x_car_data, y_car_data, x_sensed_data, y_sensed_data):
            writer.writerow([car_x, car_y, sensed_x, sensed_y])  # Write coordinate pairs for both categories
    
    print("Data saved to 'map.csv'")
    
    plt.ioff()  # Turn off interactive mode
    plt.close(fig)  # Close the figure

def read_map(map_file):
    global x_car_data, y_car_data, x_sensed_data, y_sensed_data
    
    try:
        with open(map_file, mode="r") as file:
            reader = csv.reader(file)
            header = next(reader)  # Skip the header row
            x_car_data, y_car_data, x_sensed_data, y_sensed_data = [], [], [], []  # Clear existing data
            
            for row in reader:
                car_x, car_y, sensed_x, sensed_y = map(float, row)  # Convert to float for precision
                x_car_data.append(car_x)
                y_car_data.append(car_y)
                x_sensed_data.append(sensed_x)
                y_sensed_data.append(sensed_y)
        
        print(f"Map '{map_file}' loaded successfully.")
        update_graph()  # Update the graph with new data
        plt.show(block=True)
    except FileNotFoundError:
        print(f"Error: File '{map_file}' not found.")
    except Exception as e:
        print(f"Error: Could not read map file '{map_file}'. Reason: {e}")