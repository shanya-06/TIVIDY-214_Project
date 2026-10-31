# use ubuntu as the base image
FROM ubuntu:latest

# install c++ compiler, make and doctest
RUN apt-get update && \
    apt-get install -y g++ make doctest-dev && \
    rm -rf /var/lib/apt/lists/*

# set the working directory
WORKDIR /app

# copy the project files
COPY . .

# compile the project
RUN make

# expose the gui port
EXPOSE 8214

# start the gui without opening a browser
CMD ["./tividy_gui", "--no-browser", "--port", "8214"]
