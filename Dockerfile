FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        cmake \
        g++ \
        libboost-program-options-dev \
        libgtest-dev \
        libmodbus-dev \
        make \
        pkg-config && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy project files
COPY . /app

# Build the project
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --parallel

# Run the unit tests as part of the image build
RUN ctest --test-dir build --output-on-failure

# This container just compiles the code, so there's no need to run anything
CMD ["echo", "Compilation complete. Binaries are in /app/build."]
