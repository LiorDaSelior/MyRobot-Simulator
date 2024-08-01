#include <utility>

struct IntegerPairComparator {
    bool operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const {
        if (lhs.first == rhs.first && lhs.second == rhs.second) {
            return true;
        }
        return false;
        }
    };

struct PairHasher {
    template <class T1, class T2>
    std::size_t operator ()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        //return h1 ^ (h2 << 1); 
        return h1 ^ h2; 
    }
};