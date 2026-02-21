# Huffman Studio Pro

**Huffman Studio Pro** is a high-performance, web-based file compression utility. [cite_start]It leverages **C++** and **Huffman Coding** algorithms compiled to **WebAssembly (WASM)** to provide near-native processing speeds directly in the browser[cite: 1, 2].

[cite_start]The application allows users to compress `.txt` files into a custom binary `.bin` format and restore them back to their original state with bit-perfect accuracy[cite: 2].

## 📦 How to Run

[cite_start]The project is fully containerized using **Docker** for easy deployment[cite: 1].

### Prerequisites
* [Docker](https://www.docker.com/get-started) installed on your machine.

### Instructions

1.  **Clone the Repository**
    ```bash
    git clone [https://github.com/your-username/huffman-studio-pro.git](https://github.com/your-username/huffman-studio-pro.git)
    cd huffman-studio-pro
    ```

2.  **Build the Docker Image**
    [cite_start]The Dockerfile uses a multi-stage build: it first compiles the C++ code using `emscripten/emsdk` and then sets up a lightweight Python 3.11-slim image for the runtime[cite: 1, 2].
    ```bash
    docker build -t huffman-studio .
    ```

3.  **Run the Container**
    [cite_start]The server is configured to expose and run on port **8000**[cite: 4].
    ```bash
    docker run -p 8000:8000 huffman-studio
    ```

4.  **Access the App**
    Open your browser and navigate to:
    `http://localhost:8000`

---

## 📂 Project Structure

* [cite_start]`main.cpp`: The core Huffman algorithm logic[cite: 2].
* [cite_start]`index.html`: The user interface and WASM initialization logic[cite: 2].
* [cite_start]`app.js`: Handles file reading, memory allocation (`_malloc`), and downloading results[cite: 2].
* [cite_start]`Dockerfile`: Multi-stage build instructions for Emscripten and Python[cite: 1, 2].
