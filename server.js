const express = require('express');
const path = require('path');
const bodyParser = require('body-parser');
const app = express();
const server = require('http').Server(app);
const io = require('socket.io')(server);

const port = 3000; // Adjust the port if necessary

// Middleware to parse JSON and form data
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

// Serve static files (e.g., HTML, CSS)
app.use(express.static(path.join(__dirname, 'public')));

// Endpoint to receive JSON data from ESP8266

app.post('/data', (req, res) => {
    console.log('Received data from ESP8266:');
    console.log(req.body); // Print the received data

    // Emit the received data through WebSocket
    io.emit('latestData', req.body);

    // Send the received data back to the client-side JavaScript
    res.json(req.body);
});

// Endpoint to handle form submission and login
app.post('/login', (req, res) => {
    const { id, password } = req.body;
    
    // Validate the ID and password (you can add your validation logic here)
    if (id === 'root' && password === 'root') {
        // Redirect to home page upon successful login
        res.redirect('/home');
    } else {
        // Redirect to login failed page if login fails
        res.redirect('/login-failed');
    }
});

// Routes
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'views', 'login.html'));
});

// Endpoint to serve the home HTML file
app.get('/home', (req, res) => {
    res.sendFile(path.join(__dirname, 'views', 'home.html'));
});

// Start the server
server.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
});
