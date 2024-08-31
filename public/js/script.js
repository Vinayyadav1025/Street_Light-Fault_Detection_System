document.addEventListener('DOMContentLoaded', function () {

    // Function to create buttons and map IDs
    function createButtons() {
        const container = document.getElementById('button-container');

        for (let i = 1; i <= 12; i++) {
            const button = document.createElement('div');
            button.classList.add('button');
            button.innerText = i;
            button.setAttribute('id', `button-${i}`);
            button.addEventListener('click', () => {
                // Fetch data when button is clicked
                fetchLatestData();
            });

            container.appendChild(button);
        }
    }

    // Call the function to create buttons
    createButtons();

    // Function to handle the button color based on fault ID
    // Function to handle the button color based on fault ID
    function handleButtonColor(data) {
        // Reset button color for all buttons
        const buttons = document.querySelectorAll('.button');
        buttons.forEach(btn => btn.classList.remove('faulty'));

        let faultType = '';

        if (data.ldr === 'HIGH') {
            

            if (data.current <= 0 && data.voltage <= 0) {
                faultType = 'Cable Break';
            } else if (data.voltage < 210 && data.current > 5) {
                faultType = 'Short Circuit';
            } else if (data.current > 5 && (data.voltage > 210 && data.voltage < 250)) {
                faultType = 'Current Leakage';
            } else if (data.current <= 0 && data.voltage <= 210) {
                faultType = 'Malfunctioning Power Source';
            }
        }

        console.log('Fault Type:', faultType); // Debugging statement

        // Set button color to red if the ID matches the button ID
        const button = document.getElementById(`button-${data.Id}`);
        if (button) {
            button.classList.add('faulty');

            // Add click event listener to redirect to display.html
            button.addEventListener('click', () => {
                redirectToDisplay();
            });
        }

        // Log the faultType to the console
        console.log(`Fault Type: ${faultType}`);

        // Send the data including faultType to the server
        sendDataToServer({ ...data, faultType });
    }


    // Function to fetch the latest data and handle button color
    // Function to fetch the latest data and handle button color
    function fetchLatestData() {
    fetch('/data', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        }
    })
    .then(response => response.json())
    .then(data => {
        console.log('Received latest data:', data);
        let faultType = '';

        if (data.ldr === 'HIGH') {
            console.log('ldr value is HIGH'); // Debugging statement
            

            if (data.current <= 0 && data.voltage <= 0) {
                faultType = 'Cable Break';
            } else if (data.voltage < 210 && data.current > 5) {
                faultType = 'Short Circuit';
            } else if (data.current > 5 && (data.voltage > 210 && data.voltage < 250)) {
                faultType = 'Current Leakage';
            } else if (data.current <= 0 && data.voltage <= 210) {
                faultType = 'Malfunctioning Power Source';
            }
        }

        console.log('Calculated fault type:', faultType); // Debugging statement
        handleButtonColor({ ...data, faultType });
    })
    .catch(error => console.error('Error fetching latest data:', error));
    }


    // Set up an interval to fetch data and update button color every 5 seconds
    setInterval(fetchLatestData, 5000);

    // Set up a WebSocket connection
    const socket = io();

    // Listen for 'latestData' events from the server
    socket.on('latestData', (data) => {
        
        console.log('Received latest data from server :', data);
        handleButtonColor(data);
    });

    // Function to redirect to display.html
    function redirectToDisplay() {
        window.location.href = '../css/display.html';
    }
});


