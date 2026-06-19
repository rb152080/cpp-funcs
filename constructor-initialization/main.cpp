#include <vector>

class ScoreTracker
{
  private:
    std::vector<int> scores;

  public:
    // {} is bad with std::vector because it will think its 2 elements: { size,
    // 0 }
    // ScoreTracker(int size) : scores { size, 0 } {}
    // here () will initialize the size of the std::vector to size and zero out
    // the elements
    ScoreTracker(int size) : scores(size, 0) {}
};

int main() { return 0; }
