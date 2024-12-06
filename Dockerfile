FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    apt-get install -y cmake g++ pkg-config libmodbus-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy project files
COPY . /app

# Create build directory
RUN mkdir -p build

# Build the project
WORKDIR /app/build
RUN cmake .. && \
    cmake --build .

# This container just compiles the code, so there's no need to run anything
CMD ["echo", "Compilation complete. Check the output directory on the host."]
