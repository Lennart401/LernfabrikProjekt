# LernfabrikProjekt
Top-level repository for all my Lernfabrik related work at Leuphana University.

## Running in production / for a Lernfabrik Workshop

In order to run the AI Kanban containers in a production Lernfabrik workshop, the computer that runs the server and web software must be connected to the same network as the boxes. On the boxes, the IP address of the machine running the server must be set correctly, otherwise they won't be able to connect to the MQTT broker.

Running the production servers can be easily done using Docker Compose, by running `docker compose up --build -d`. Docker Compose then starts a MQTT broker on port 1883, the realtime tracker and websocket server on port 8765 and the web frontend on port 3000. They automatically connect and work without any manual configuration.

Open [localhost:3000](http://localhost:3000) for the view of all the boxes.


## Repository Structure

This structure of the code is a follows:

### Arduino

This folder contains all code that needs to run on the Arduino Portenta H7 hardware. It is fairly well documented, so check its [README](arduino/README.md) and [MANUAL](arduino/MANUAL.md) for description of its code.

### Data Collector

The data collector all the software that runs again production boxes, i.e. boxes that have a trained ML model and are in production mode. See its [README](data-collector/README.md) for a detailed description of its contents.

### Data Server

When no ML model has been trained and raw acceleration data must be collected, this is the way to go. Essentially, the data server is just a simple HTTP server that collects and stores data from the boxes. The boxes must be in recording mode for this. The details about how the data server and the underlying protocol works are documented in its [README](data-server/README.md).

### Machine Learning

This folder contains all sorts of code that does all sorts of ML related stuff, i.e. data exploration, data cleaning, training and finally deployment of the model to a C-compatible header file. There is a small [README](machine-learning/README.md) which roughly explains the process, but for the most part the code speaks for itself.

### NextJS Frontend

The web frontend for production use in a Lernfabrik Workshop is written in NextJS and can easily be deployed using Docker. It is a standard NextJS project created with `create-next-app`. It contains only a single page, two components for displaying the boxes and a hook that connect to a TCP server using websockets for getting updates from the boxes.
