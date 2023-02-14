FROM ubuntu:latest

RUN apt-get update && \
  apt-get install -y xorg-dev curl cmake make git && \
  rm -rf /var/lib/apt/lists/*

WORKDIR /b

COPY . .

COPY CMakeLists.txt .

RUN git submodule update --init

RUN mkdir build && cd build && cmake .. && make

RUN chmod +x ./b 

CMD ["./b"]
