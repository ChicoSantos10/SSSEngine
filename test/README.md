# Testing

To ensure that the code does what we think it does, we will write unit tests. 

## Coverage

We will not strive to maintain any sort of coverage. Instead, we will focus in making sure the things that should be
tested are tested. For example, everything in Math library will be tested. However, other libraries may not achieve full
coverage. Instead, some logic will be done with runtime and compile time checks with assertions. 

## Folder structure

Each library will have its folder and be its own test. Each file should be the same name as the file it is testing but
with added .test. As an example when testing Matrix we create a Matrix.test.cpp file.
