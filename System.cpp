#include "System.h"

using namespace std;

void System::Tick(void) {
    cout << " ************************ TICK " << tickNumber++ << " ************************" << endl;
    for (int i = processors->size() - 1; i >=0 ; i--) {
        Processor* p = processors->at(i);
        //cout << "Processor " << i << "->Tick()...";
        p->Tick();
        //cout << "Complete" << endl;
    }
    //cout << "Memory->Tick()...";
    memory->Tick();
    //cout << "Complete" << endl;
    //cout << "Interconnect->Tick()...";
    interconnect->Tick();
    //cout << "Complete" << endl;
}

void System::insertInstruction(Instruction* i, unsigned int processorNum) {
    assert(processorNum < processors->size());
    Processor* p = processors->at(processorNum);
    p->processInstruction(i);
}

bool System::hasPendingInstructions(unsigned int processorNum) {
    assert(processorNum < processors->size());
    Processor* p = processors->at(processorNum);
    return p->hasPendingInstructions();
}

void System::reportMemoryOpStatistics(void) {
    unsigned long sum = 0;
    unsigned long totalHits = 0;
    unsigned long totalMisses = 0;
    for (int i =0; i <  processors->size() ; i++) {
        Processor* p = processors->at(i);
        unsigned long numOps = p->getTotalMemoryOps();
        double averageMemoryWait = p->getAverageMemoryLatency();
        unsigned long storeHits = p->getStoreHits();
        unsigned long storeMisses = p->getStoreMisses();
        unsigned long loadHits = p->getLoadHits();
        unsigned long loadMisses = p->getLoadMisses();
        totalHits = totalHits + storeHits + loadHits;
        totalMisses = totalMisses + storeMisses + loadMisses;
        double hitRate = 100.0*((double)(storeHits+loadHits))/((double)(storeHits+storeMisses+loadHits+loadMisses));
        sum += numOps;
        cout << "Processor " << i << ":" << endl;
        cout << "    Memory Operations: " << numOps << endl;
        cout << "    Average Latency  : " << averageMemoryWait   << endl;
        cout << "    Store Hits       : " << storeHits   << endl;
        cout << "    Store Misses     : " << storeMisses << endl;
        cout << "    Load Hits        : " << loadHits    << endl;
        cout << "    Load Misses      : " << loadMisses  << endl;
        cout << "    Cache Hit Rate   : " << hitRate << "%" << endl;
    }
    // Calculate overall average latency
    cout << endl;
    double overallAvgLatency = 0.0;
    for (int i =0; i <  processors->size() ; i++) {
        Processor* p = processors->at(i);
        unsigned long numOps = p->getTotalMemoryOps();
        double averageMemoryWait = p->getAverageMemoryLatency();
        overallAvgLatency += averageMemoryWait*((double)numOps)/((double)sum);
    }
    double finalHitRate = 100.0*((double)totalHits)/((double)totalHits+totalMisses);
    cout << "Summary:" << endl;
    cout << "    Total Memory Operations : " << sum << endl;
    cout << "    Overall Average Latency : " << overallAvgLatency << endl;
    cout << "    Reads Serviced By Memory: " << memory->getMemCount() << endl;
    cout << "    Total Combined Hits     : " << totalHits << endl;
    cout << "    Total Combined Misses   : " << totalMisses << endl;
    cout << "    Combined Hit Rate       : " << finalHitRate << "%" << endl;
    //interconnect->reportBusStatistics();
}