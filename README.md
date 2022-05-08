## Description
Managed DotNet wrapper for the [taskflow](https://github.com/taskflow/taskflow) C++ library
Experimenτal measures in the tester project indicate that massive task execution is slightly faster than
native .net tasks. May be helpful for those who struggle with scaling huge machine learning models or parallel programming.
You received for free, you gave it for free

## Features
This the very very fist version, there is only support for building the basic flow graph and its execution.
Though there are plans for supporting more, for now this is crafted just for fun

## Build
Since taskflow requires C\+\+20 which is not allowed in a managed C++ .net library, necessarily had to create 2
layers of wrapping over the actual taskflow library. 
Coding was quick and safe, lasted about 1 weekend, prooves that C++ is getting cool again !!

