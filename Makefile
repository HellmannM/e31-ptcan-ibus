CXX := g++
CXXFLAGS := -std=c++17 -O2
#LDFLAGS := -ltbb
MOREFLAGS := -fdiagnostics-color=always
INCLUDES := -I. -I./stubs

.PHONY: all
all: relay-box.o

.PHONY: debug
debug: CXXFLAGS += -Wall -Wpedantic -Wextra -Wno-unused-parameter -Wno-unused-but-set-variable -g -O0
debug: all

.PHONY: werror
werror: CXXFLAGS += -Werror
werror: debug

.PHONY: fatal
fatal: CXXFLAGS += -Wfatal-errors
fatal: all

relay: relay.o IbusMessage.o IbusTrx.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(MOREFLAGS) $^ -o $@

relay.o: relay.cpp IbusMessage.h IbusTrx.h IbusNames.h
	$(CXX) $(CXXFLAGS) $(MOREFLAGS) $(INCLUDES) -c relay.cpp -o $@

IbusMessage.o: IbusMessage.h IbusMessage.cpp types.h
	$(CXX) $(CXXFLAGS) $(MOREFLAGS) $(INCLUDES) -c IbusMessage.cpp -o $@

IbusTrx.o: IbusTrx.h IbusTrx.cpp IbusNames.h types.h
	$(CXX) $(CXXFLAGS) $(MOREFLAGS) $(INCLUDES) -c IbusTrx.cpp -o $@

#relay-box: relay-box.o
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(MOREFLAGS) $^ -o $@

relay-box.o: relay-box.cpp can_id.h gears.h states.h
	$(CXX) $(CXXFLAGS) $(MOREFLAGS) $(INCLUDES) -c relay-box.cpp -o $@

.PHONY: clean
clean:
	-@rm -f *.o relay

