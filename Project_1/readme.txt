There was a slight problem with this assignment.

The exit() function was meant to be used to pass the resulting value from the child to the parent process.
The issue with this is that only the lower 8 bits are passed through exit().

For a signed value or a value larger than 8 bits, the parent would print the result incorrectly.
Although not part of the class syllabus, I found that using pipes resolved this, so that's how I ended up coding it.

