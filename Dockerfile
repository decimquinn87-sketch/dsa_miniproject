# Stage 1: Build Stage (Emscripten)
FROM emscripten/emsdk:latest AS builder

WORKDIR /src

# Copy all project files
COPY . .

# VERIFICATION: Check for emcc and python3 (emsdk includes python)
RUN echo "Checking environment..." && \
    emcc --version && \
    python3 --version

# COMPILATION: Build the WASM logic
RUN emcc main.cpp -o huffman.js \
    -s WASM=1 \
    -s "EXPORTED_RUNTIME_METHODS=['ccall','HEAPU8']" \
    -s "EXPORTED_FUNCTIONS=['_compress_text','_decompress_file','_get_result_size','_get_result_ptr','_malloc','_free']" \
    -s ALLOW_MEMORY_GROWTH=1 \
    -O3

# Stage 2: Runtime Stage (Pulling official Python image)
FROM python:3.11-slim AS runtime

WORKDIR /app

# Final environment check
RUN python3 --version

# Copy compiled artifacts from builder
COPY --from=builder /src/huffman.js .
COPY --from=builder /src/huffman.wasm .
COPY --from=builder /src/app.js .
COPY --from=builder /src/index.html .

# Expose port 8000
EXPOSE 8000

# Start the Python server
# -u ensures logs are sent straight to terminal (unbuffered)
CMD ["python3", "-u", "-m", "http.server", "8000"]