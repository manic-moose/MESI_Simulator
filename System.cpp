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
    for (int i =0; i <  processors->size() ; i++) {
        Processor* p = processors->at(i);
        unsigned long numOps = p->getTotalMemoryOps();
        double averageMemoryWait = p->getAverageMemoryLatency();
        sum += numOps;
        cout << " Processor " << i << "Total Memory Operations: " << numOps << " Average Latency: " << averageMemoryWait << endl;
    }
    // Calculate overall average latency
    double overallAvgLatency = 0.0;
    for (int i =0; i <  processors->size() ; i++) {
        Processor* p = processors->at(i);
        unsigned long numOps = p->getTotalMemoryOps();
        double averageMemoryWait = p->getAverageMemoryLatency();
        overallAvgLatency += averageMemoryWait*((double)numOps)/((double)sum);
    }
    cout << "Sum: " << sum << "  Overall Average Latency: " << overallAvgLatency << endl;
    cout << "Memory Serviced Operations: " << memory->getMemCount() << endl;
    interconnect->reportBusStatistics();
    
}