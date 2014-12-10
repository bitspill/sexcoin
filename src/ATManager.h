class ATManager
{
private:
    vector<AutomatedTransaction> at_collection;

public:
    bool addAT(AutomatedTransaction);
    void singleStepAll();
    void fullStepAll();
    void removeFinished();
};