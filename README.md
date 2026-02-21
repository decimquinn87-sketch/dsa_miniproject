# Huffman Studio Pro

**Huffman Studio Pro** is a high performance, web based file compression utility. It uses **C++** and **Huffman Coding** algorithms compiled to **WebAssembly (WASM)** to provide near-native processing speeds directly in the browser

The application allows users to compress `.txt` files into a custom binary `.bin` format and restore them back to their original state with bit-perfect accuracy.

## 📦 How to Run

The project is fully containerized using **Docker** for easy deployment

### Prerequisites
* [Docker](https://www.docker.com/get-started) installed on your machine.

### Instructions

1.  **Clone the Repository**
    ```bash
    git clone [https://github.com/your-username/huffman-studio-pro.git](https://github.com/your-username/huffman-studio-pro.git)
    cd huffman-studio-pro
    ```

2.  **Build the Docker Image**
The Dockerfile uses a multi-stage build: it first compiles the C++ code using `emscripten/emsdk` and then sets up a lightweight Python 3.11-slim image for the runtime
    ```bash
    docker build -t huffman-studio .
    ```

3.  **Run the Container**
The server is configured to expose and run on port **8000**
    ```bash
    docker run -p 8000:8000 huffman-studio
    ```

4.  **Access the App**
    Open your browser and navigate to:
    `http://localhost:8000`

---

## 📂 Project Structure

* `main.cpp`: The core Huffman algorithm logic.
* `index.html`: The user interface and WASM initialization logic.
* `app.js`: Handles file reading, memory allocation (`_malloc`), and downloading results.
* `Dockerfile`: Multi-stage build instructions for Emscripten and Python
