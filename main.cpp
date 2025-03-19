#include "vector/vector.h"
#include "optional/optional.h"
#include "list/list.h"
#include "array/array.h"


int main() {
    self::Vector<int> vec{10UL};
    self::Optional<double> val{1.12};
    self::Array<float, 10UL> arr;
    self::SingleLinkedList<int> list = {1, 2, 3};
}