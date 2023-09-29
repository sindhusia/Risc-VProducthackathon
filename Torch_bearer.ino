#include <stdint.h>

// Define the memory-mapped display hardware addresses
#define DISPLAY_BASE_ADDR 0xFFFFFF //  actual address
#define DISPLAY_WIDTH 320          //  actual width
#define DISPLAY_HEIGHT 240         //  actual height

// Define the maximum number of coordinates to save
#define MAX_COORDINATES 1000

const int buzzerPin = 9; //  pin number where your buzzer is connected

// Function to control the buzzer and generate a tone
void triggerBuzzer() {
    digitalWrite(buzzerPin, HIGH); // Turn ON the buzzer
    delay(1000); // delay for tone duration
    digitalWrite(buzzerPin, LOW); // Turn OFF the buzzer
    delay(1000); // delay for tone off duration

void setup() {
    // Initialize the buzzer pin as an OUTPUT
    pinMode(buzzerPin, OUTPUT);
}

// Structure to store GPS coordinates
typedef struct {
    int latitude;
    int longitude;
} GPSCoordinate;

// Function to draw a pixel at (x, y) on the display
void drawPixel(int x, int y, uint16_t color) {
    // address of the pixel in memory
    uint32_t addr = DISPLAY_BASE_ADDR + (y * DISPLAY_WIDTH + x) * 2;

    // Write the pixel color to the address
    *(volatile uint16_t *)addr = color;
}

// Function to read GPS coordinates 
void readGPS(int *latitude, int *longitude) {
    // Simulated GPS data for testing
    *latitude = rand() % DISPLAY_WIDTH;
    *longitude = rand() % DISPLAY_HEIGHT;
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Radius of the Earth in meters
    double earthRadius = 6371000.0; // Approximate value for Earth's radius

    // Conversion latitude and longitude from degrees to radians
    lat1 = lat1 * M_PI / 180.0;
    lon1 = lon1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    lon2 = lon2 * M_PI / 180.0;

    // Haversine formula
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    double a = sin(dlat / 2.0) * sin(dlat / 2.0) + cos(lat1) * cos(lat2) * sin(dlon / 2.0) * sin(dlon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    double distance = earthRadius * c;

    return distance;
}

int main() {
    // Initialize Riscduino hardware and display
    // Initialize GPS module (We'll need to implement this on Riscduino)

    uint32_t startTime = 0;
    uint32_t updateTime = 10000; // Update every 10 seconds

    int latitude, longitude;

    // Create an array to store GPS coordinates
    GPSCoordinate coordinates[MAX_COORDINATES];
    int coordinateCount = 0;

    while (1) {
        // Check if it's time to update coordinates
        uint32_t currentTime = millis();
        if (currentTime - startTime >= updateTime) {
            // Read GPS coordinates
            readGPS(&latitude, &longitude);

            // Save the coordinates in the array
            if (coordinateCount < MAX_COORDINATES) {
                coordinates[coordinateCount].latitude = latitude;
                coordinates[coordinateCount].longitude = longitude;
                coordinateCount++;
            }

            // Clear the display
            for (int y = 0; y < DISPLAY_HEIGHT; y++) {
                for (int x = 0; x < DISPLAY_WIDTH; x++) {
                    drawPixel(x, y, 0x0000); // Set pixel color to black
                }
            }

            // Plot coordinates
            drawPixel(latitude, longitude, 0xFFFF); // Set pixel color to white

            // Check for movement warning
            if (coordinateCount > 1) {
                double distance = calculateDistance(
                    coordinates[coordinateCount - 2].latitude,
                    coordinates[coordinateCount - 2].longitude,
                    latitude,
                    longitude
                );
                if (distance < 0.000001) {
                    
                    triggerBuzzer();//Trigger the buzzer
                    
                }
            }

            
            startTime = currentTime;// Update the start time
        }
    }
}

