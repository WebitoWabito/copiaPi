CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lssl -lcrypto

BUILD = build

SERVER_BIN = $(BUILD)/server
CLIENT_BIN = $(BUILD)/cliente

COMMON_OBJS = $(BUILD)/VSocket.o $(BUILD)/Socket.o $(BUILD)/SSLSocket.o
SERVER_OBJS = $(COMMON_OBJS) $(BUILD)/servidorFiguras.o $(BUILD)/FileMon.o
CLIENT_OBJS = $(COMMON_OBJS) $(BUILD)/Cliente.o

.PHONY: help all server client run-server run-client clean

help:
	@echo "Comandos disponibles:"
	@echo "  make all          -> compila servidor y cliente"
	@echo "  make server       -> compila solo el servidor"
	@echo "  make client       -> compila solo el cliente"
	@echo "  make run-server   -> ejecuta el servidor"
	@echo "  make run-client ARGS="IP puerto"   -> ejecuta el cliente"
	@echo "  make clean        -> elimina archivos compilados"

all: server client

server: $(SERVER_BIN)

client: $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(SERVER_OBJS) -o $@ $(LDFLAGS)

$(CLIENT_BIN): $(CLIENT_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CLIENT_OBJS) -o $@ $(LDFLAGS)

$(BUILD)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run-server: server
	@./$(SERVER_BIN) $(ARGS)

run-client: client
	@./$(CLIENT_BIN) $(ARGS)

clean:
	rm -rf $(BUILD)