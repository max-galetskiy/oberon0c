# Start from a Debian base image
FROM debian:bullseye-slim

# Set environment variable to non-interactive to prevent prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies for building CMake
RUN apt update && \
    apt install -y software-properties-common lsb-release g++ llvm libboost-all-dev wget build-essential libssl-dev && \
    apt clean && rm -rf /var/lib/apt/lists/*

# Download and install CMake from source
RUN wget https://github.com/Kitware/CMake/releases/download/v3.30.5/cmake-3.30.5.tar.gz && \
    tar -zxvf cmake-3.30.5.tar.gz && \
    cd cmake-3.30.5 && \
    ./bootstrap && \
    make -j$(nproc) && \
    make install && \
    cd .. && rm -rf cmake-3.30.5* && \
    cmake --version

# Set up the working directory
WORKDIR /usr/src/app

# Copy application files
COPY . .

# Build the project
RUN mkdir build && cd build && cmake .. && cmake --build .

# Default to bash for the container's entry point
CMD ["bash"]
