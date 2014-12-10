#include <vector>
#include "at.h"
#include "ATManager.h"

#define STEPS_PER_BLOCK (5)

using namespace std;

bool ATManager::addAT(/* some info extracted from a transaction */) {
    // AutomatedTransaction at = new AutomatedTransaction(int8_t* p_code, int32_t csize, int8_t* p_data, int32_t dsize, int32_t cssize, int32_t ussize);
    return false;
}

void ATManager::singleStepAll() {
    for (std::vector<AutomatedTransaction>::iterator it = at_collection.begin(); it != at_collection.end(); ++it)
        it->processOp();
}

void ATManager::removeFinished() {
    std::vector<AutomatedTransaction>::iterator it = at_collection.begin();
    while(it != at_collection.end())
    {
        if(it->isFinished())
            it = at_collection.erase(it);
        else
            ++it;
    }
}

void ATManager::fullStepAll() {
    for (int i = 0; i < STEPS_PER_BLOCK; ++i) {
        this->singleStepAll();
    }
}