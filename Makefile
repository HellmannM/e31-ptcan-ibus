CXX := g++
CXXFLAGS := -std=c++17 -O2
LDFLAGS := -ltbb
MOREFLAGS := -fdiagnostics-color=always
INCLUDES := -I. -I./stubs

.PHONY: all
all: relay

.PHONY: debug
debug: CXXFLAGS += -Wall -Wpedantic -Wextra -Wno-unused-parameter -Wno-unused-but-set-variable -g -O0
debug: all

.PHONY: werror
werror: CXXFLAGS += -Werror
werror: debug

relay: relay.o IbusMessage.o IbusTrx.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(MOREFLAGS) $^ -o $@

relay.o: relay.cpp IbusMessage.h IbusTrx.h IbusNames.h
	$(CXX) $(CXXFLAGS) $(MOREFLAGS) $(INCLUDES) -c relay.cpp -o $@

IbusMessage.o: IbusMessage.h IbusMessage.cpp types.h
	$(CXX) $(CXXFLAGS) $(MOREFLAGS) $(INCLUDES) -c IbusMessage.cpp -o $@

IbusTrx.o: IbusTrx.h IbusTrx.cpp IbusNames.h types.h
	$(CXX) $(CXXFLAGS) $(MOREFLAGS) $(INCLUDES) -c IbusTrx.cpp -o $@

.PHONY: clean
clean:
	-@rm -f *.o relay

