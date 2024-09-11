const int analogInputPin = A0;  // Analog input pin for voltage sensor
const float voltageScalingFactor = 0.071;  // Scaling factor for 220V input

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Number of samples (adjust as needed)
  const int numSamples = 100;

  // Array to store voltage samples
  float voltageSamples[numSamples];

  // Read voltage samples
  for (int i = 0; i < numSamples; i++) {
    // Read analog voltage from voltage sensor
    int sensorValue = analogRead(analogInputPin);

    // Convert analog value to voltage (in volts)
    float voltage = sensorValue * (5.0 / 1024.0);

    // Scale the voltage based on the input range (220V)
    voltage = voltage * voltageScalingFactor;

    // Store the squared value in the array
    voltageSamples[i] = voltage * voltage;

    delay(10);  // Adjust delay based on your sampling requirements
  }

  // Calculate the mean of the squared values
  float sumOfSquares = 0;
  for (int i = 0; i < numSamples; i++) {
    sumOfSquares += voltageSamples[i];
  }
  float meanOfSquares = sumOfSquares / numSamples;

  // Calculate the RMS value
  float rmsValue = sqrt(meanOfSquares);

  // Print RMS value to Serial Monitor
  Serial.print("RMS Voltage: ");
  Serial.print(rmsValue, 3);  // Display with three decimal places
  Serial.println(" V");

  delay(1000);  // Adjust delay for your specific requirements
}
