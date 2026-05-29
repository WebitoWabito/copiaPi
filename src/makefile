CXX := g++
CXXFLAGS := -g -Wall -std=c++17

DEPS := VSocket.h SSLSocket.h

TARGET := client

# Regla para .cc
%.o: %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar el cliente
client: client.o VSocket.o SSLSocket.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lssl -lcrypto

# Ejecutar
run:
	./client

# Limpiar
clean:
	rm -f *.o $(TARGET)


	

## THIS IS FOR MAC

# OPENSSL_PATH := $(shell brew --prefix openssl@3)

# CXX := g++
# CXXFLAGS := -g -Wall -std=c++17 -I$(OPENSSL_PATH)/include
# LDFLAGS := -L$(OPENSSL_PATH)/lib -lssl -lcrypto

# DEPS := VSocket.h SSLSocket.h
# OBJS := client.o VSocket.o SSLSocket.o

# TARGET := client

# # Regla genérica
# %.o: %.cc $(DEPS)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# # Ejecutable
# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# # Ejecutar
# run: $(TARGET)
# 	./$(TARGET)

# # Limpiar
# clean:
# 	rm -f *.o $(TARGET)
# 	rm -rf *.dSYM