# PL605

## Build
```
mkdir build && cd build
cmake ../
make
```

## Run
```
./pl605 <filename>
```

## Execute
```
cd runtime
java TestCompiler <code file> <data file>
```
### Arguments
-a: Arithmetic Simplification
-c: Constant Folding
-o: Orphan Function Elimination
-r <int>: Number of register for allocation