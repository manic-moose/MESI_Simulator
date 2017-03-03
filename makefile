

all: System.cpp Processor.cpp Memory.cpp Interconnect.cpp BusNode.cpp CacheController.cpp MI_Controller.cpp MSI_Controller.cpp MESI_Controller.cpp Cache.cpp CacheSet.cpp CacheLine.cpp cache_coherence_sim.cpp
	g++ System.cpp Processor.cpp Memory.cpp Interconnect.cpp BusNode.cpp CacheController.cpp MI_Controller.cpp MSI_Controller.cpp MESI_Controller.cpp Cache.cpp CacheSet.cpp CacheLine.cpp cache_coherence_sim.cpp -o cache_coherence_sim
