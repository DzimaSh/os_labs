struct ArrData //structure that stores a pointer of array and its size
{
    int N = 0; //array size
    int* arr = nullptr;
    
    ArrData(int N) {
        this->N = N;
        arr = new int[N];
    }
    ~ArrData() {
        delete[] arr;
        arr = nullptr;
    }
};
