# 1) Build stage
FROM ubuntu:22.04 AS build

# Install build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside the image
WORKDIR /app

# Copy the whole project
COPY . .

# Configure and build with CMake
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build --config Release --target NecroClient -j$(nproc)

# 2) Runtime stage: minimal image
FROM ubuntu:22.04 AS runtime

# For logging / diagnostics if needed
RUN apt-get update && apt-get install -y \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the built binary from the build stage
COPY --from=build /app/build/NecroClient /app/NecroClient

# Expose HTTP port
EXPOSE 8080

# Run the server
CMD ["./NecroClient"]