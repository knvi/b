FROM ubuntu:latest

RUN apt-get update && \
  apt-get install -y libglfw3-dev xorg-dev curl cmake make git && \
  rm -rf /var/lib/apt/lists/*

WORKDIR /b

COPY . .

COPY CMakeLists.txt .

RUN git submodule update --init

RUN mkdir build && cd build && cmake .. && make

ENV DISPLAY=:0

CMD ["./build/b"]
