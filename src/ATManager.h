#include <vector>
#include "at.h"

class ATManager
{
private:
    std::vector<AutomatedTransaction> at_collection;

public:
    bool addAT(AutomatedTransaction);
    void singleStepAll();
    void fullStepAll();
    void removeFinished();
};