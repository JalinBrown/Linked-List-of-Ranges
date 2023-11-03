# RangeList C++ Class

**Author:** Jalin Brown
**Email:** JalinBrownWorks@gmail.com

## Brief Description

The `RangeList` C++ class is a versatile implementation of a range-based list. It allows you to manage ranges of values and perform various operations on them. The class is designed to handle edge cases and offers features like the Proxy design pattern to support index operators, scalar multiplication, and vector addition.

## Features

- **Range Management:** The `RangeList` class allows you to insert, delete, and modify ranges of values. It handles various edge cases, ensuring that ranges are correctly managed.

- **Mathematical Operations:** You can perform mathematical operations between two `RangeList` objects, such as multiplication, to compute the product of values in overlapping ranges.

- **Proxy Design Pattern:** The class implements the Proxy design pattern to support index operators (`operator[]`), enabling you to access values within the ranges using subscript notation.

- **Edge Case Handling:** The class is designed to handle edge cases effectively. It correctly merges or splits ranges as needed when inserting or modifying ranges.

## Usage

To use the `RangeList` class, follow these steps:

1. Include the "RangeList.h" header in your C++ project.

2. Create instances of `RangeList` to work with ranges.

3. Use the `insert_value` method to insert ranges into the `RangeList`. If a new range partially overlaps with an existing range, it will be handled correctly by merging or splitting as necessary.

4. Perform mathematical operations between `RangeList` objects using the provided operators, such as multiplication.

## Example

Here's an example of using the `RangeList` class to find the product of two ranges:

```cpp
#include "RangeList.h"

int main() {
    // Create two RangeLists
    RangeList rl1;
    RangeList rl2;

    // Insert ranges into the lists
    rl1.insert_value(0, 10, 2);
    rl2.insert_value(5, 15, 3);

    // Multiply the two RangeLists
    int product = rl1 * rl2;
    std::cout << "Product: " << product << std::endl;

    return 0;
}
```
