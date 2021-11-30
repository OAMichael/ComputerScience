# Programs which interate mathematical function

### Both programs calculate definite integral of mathematical function and plot a graph of it. The difference is that `14-Integrate.exe` uses one, main thread, but `14-IntegrateThreads.exe` uses multiple number of threads

#### Both programs `14-Integrate.exe` and `14-IntegrateThreads.exe` take as arguments:
1. Mathematical function bounded with `''` and explicit argument in parentheses. For example, `'sin(x)'`
2. Decimal number with floating point - start of intergration interval
3. Decimal number with floating point - end of integration interval
4. Number of points to be used to calculate integral and plot a graph

## Example:

```console
$ ./14-Integrate.exe 'sin(x)' 0 3.1416 1000
```


#### `14-IntegrateThreads.exe` can also take a number of threads to perform a calculation like that:
```console
$ ./14-IntegrateThreads.exe 'sin(x)' 0 3.1416 1000 8
```

#### But it is optionally, by default number of threads is 4 (including main one)
